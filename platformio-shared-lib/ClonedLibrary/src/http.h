

#ifndef HTTP_SERVER_C
#define HTTP_SERVER_C
#include "arduinoref.h"
#include <HTTPClient.h>
#include "http_request.h"

class HttpServer
{
public:
  void step();
  HttpServer(int port, std::function<String(HttpRequest *)> callback);
  std::function<String(HttpRequest *)> requestHandle;
  void begin();

private:
  WiFiServer wifiServer;
  // Current time
  unsigned long currentTime = millis();
  // Previous time
  unsigned long previousTime = 0;
  // Define timeout time in milliseconds (example: 2000ms = 2s)
  const long timeoutTime = 3000;

  void processRequest();
  void parseRequest(WiFiClient *client);
};

#endif