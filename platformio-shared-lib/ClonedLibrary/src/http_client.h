
#ifndef CSTM_HTTP_REQUEST_CLI
#define CSTM_HTTP_REQUEST_CLI
#include <map>
#include <HTTPClient.h>

class HttpClientRequest
{
public:
  HttpClientRequest();
  HttpClientRequest(std::string url);

  void send();
  void jsonBody(std::string json);
  void txtBody(String *txt);
  std::string method = "POST";
  std::string url;
  void addHeader(std::string, std::string);
  std::map<std::string, std::string> headers = {};
  std::map<std::string, std::string> responseHeaders = {};

  String *body = nullptr;
  std::string responseBody;

  WiFiClient *wifiClient;
  int responseStatus = 0;

  void (*callback)(HttpClientRequest *request, int status, std::string responseBody);
  ~HttpClientRequest()
  {
    delete body;
  }

private:
};

#endif