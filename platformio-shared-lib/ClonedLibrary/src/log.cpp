#include "jsonnode.h"
#include <map>
#include "arduinoref.h"
#include "prop.h"
#include "http_client.h"
#include "encoding.h"
#include "wi_fi.h"

void logResponse(HttpClientRequest *request, int status, std::string response)
{
  Serial.println(String(status));
  Serial.println(response.c_str());
}

void logRaw(std::string logLevel, std::string message, const std::map<std::string, std::string> headers, String *value)
{
  waitForWifi();

  std::string logUrl = logEndpoint() + "/tm/libs/log/raw";

  HttpClientRequest request(logUrl);

  for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
  {
    request.addHeader("log-" + it->first, it->second);
  }
  request.addHeader("log-message", message);
  request.addHeader("log-Severity", logLevel);
  request.addHeader("log-application", getDeviceKey().c_str());
  request.addHeader("log-ip", getDeviceIp());

  request.txtBody(value);
  // Serial.println(("log request:" + request.body).c_str());
  request.callback = logResponse;
  request.send();
}

void logData(std::string logLevel, std::string message, const std::map<std::string, std::string> data)
{

  waitForWifi();
  std::map<std::string, JsonNode> reqestData;

  reqestData.insert(std::pair<std::string, JsonNode>("message", JsonFactory::str(message)));
  reqestData.insert(std::pair<std::string, JsonNode>("Severity", JsonFactory::str(logLevel)));
  reqestData.insert(std::pair<std::string, JsonNode>("application", JsonFactory::str(getDeviceKey().c_str())));
  reqestData.insert(std::pair<std::string, JsonNode>("ip", JsonFactory::str(getDeviceIp())));
  int freesize = esp_get_free_heap_size();
  reqestData.insert(std::pair<std::string, JsonNode>("freemem", JsonNode(freesize)));

  for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it)
  {
    reqestData.insert(std::pair<std::string, JsonNode>(it->first, JsonFactory::str(it->second)));
  }
  JsonNode obj = JsonFactory::obj(reqestData);
  Serial.print("json size:");
  Serial.println(sizeof(obj));

  std::string logUrl = logEndpoint() + "/tm/libs/log/index.php";

  Serial.print("req url:");
  Serial.println(logUrl.c_str());
  HttpClientRequest request(logUrl);
  std::string json_data = stringify(obj);

  request.body = new String(base64_encode(json_data).c_str());
  // Serial.println(("log request:" + request.body).c_str());
  request.callback = logResponse;
  request.send();
}

void negativeResponseLogger(HttpClientRequest *request, int code, std::string data)
{
  Serial.println("triggerh");
  if (code != HTTP_CODE_OK && code != 409)
  {
    if (request->url.find("log/index.php") > -1)
    {
      return;
    }

    Serial.print("got error response code:");
    Serial.println(code);
    Serial.println(data.c_str());
    logData("ERROR", "error in request", {//
                                          {"application", getDeviceKey()},
                                          {"requrl", request->url},
                                          {"code", String(code).c_str()},
                                          {"reqbody", request->body->c_str()},
                                          {"response_data", data}});
  }
}