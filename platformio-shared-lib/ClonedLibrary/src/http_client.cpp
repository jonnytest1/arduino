#include "http_client.h"
#include "wi_fi.h"
#include "str.h"

HttpClientRequest::HttpClientRequest()
{
}
HttpClientRequest::HttpClientRequest(std::string reqUrl)
{
  url = reqUrl;
}

void HttpClientRequest::addHeader(std::string key, std::string value)
{
  std::pair<std::string, std::string> pair(key, value);
  headers.insert(pair);
}
void HttpClientRequest::jsonBody(std::string json)
{
  std::pair<std::string, std::string> pair("content-type", "application/json");
  headers.insert(pair);

  body = json;
}

void HttpClientRequest::txtBody(std::string txt)
{
  addHeader("content-type", "text/plain");

  body = txt;
}
void HttpClientRequest::send()
{
  waitForWifi();
  HTTPClient httpf;

  Serial.print("reuqest to");
  Serial.println(url.c_str());
  httpf.begin(toArduinoString(url.c_str()));

  for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
  {
    httpf.addHeader(toArduinoString(it->first), toArduinoString(it->second));
  }

  String reqBody = body.c_str();
  Serial.println(reqBody);

  int httpCodef = httpf.sendRequest(method.c_str(), reqBody);

  if (httpCodef > 0)
  {

    Serial.printf("[HTTP] resppnse code: %d\n", httpCodef);
    if (callback != NULL)
    {
      responseBody = httpf.getString().c_str();
      callback(this, httpCodef, responseBody);
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", httpf.errorToString(httpCodef).c_str());
  }
  httpf.end();
}
