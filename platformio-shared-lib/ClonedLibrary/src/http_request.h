
#ifndef CSTM_HTTP_REQUEST
#define CSTM_HTTP_REQUEST
#include <map>
#include <HTTPClient.h>

class HttpRequest
{
public:
    HttpRequest(String header, WiFiClient *client, std::function<String(HttpRequest *)> callback);

    void sendHeader(int status, int size);
    void sendResponse(String body);

    String method;
    String path;
    std::map<String, String> headers = {};
    std::map<std::string, std::string> responseHeaders = {};
    String requestHeader;
    String body;
    WiFiClient *wifiClient;
    int responseStatus = 404;
    bool asyncSend = false;

private:
    std::function<String(HttpRequest *)> requestHandle;

    bool hasSentHeader;

    void handleRequest();
    void parseRequestHeader();
    void parseRequestBody();
    void parseHttpHeader(String header);
};

#endif