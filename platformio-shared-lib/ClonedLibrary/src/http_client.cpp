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

  body = new String(json.c_str());
}

void HttpClientRequest::txtBody(String *txt)
{
  addHeader("content-type", "text/plain");

  body = txt;
}
void HttpClientRequest::send()
{
  waitForWifi();
  HTTPClient httpf;

  Serial.print("reuqest to ");
  Serial.println(url.c_str());
  if (body != nullptr)
  {
    Serial.print("with size ");
    Serial.println(body->length());
  }

  httpf.begin(toArduinoString(url.c_str()));
  for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
  {
    // Serial.println("adding header: ");
    // Serial.println(it->first.c_str());
    // Serial.println(it->second.c_str());

    httpf.addHeader(toArduinoString(it->first), toArduinoString(it->second));
  }
  if (body == nullptr)
  {
    body = new String();
  }
  int httpCodef = httpf.sendRequest(method.c_str(), *body);

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

    Serial.println(WiFi.dnsIP(0));
    Serial.println(WiFi.dnsIP(1));
    Serial.printf("[HTTP] request ... failed, error: %s\n", httpf.errorToString(httpCodef).c_str());
  }
  httpf.end();
}
