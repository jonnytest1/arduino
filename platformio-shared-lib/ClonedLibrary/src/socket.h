#ifndef C_SOCKET
#define C_SOCKET

#include "str.h"
#include <WiFi.h>
#include <sstream>

class CSOcket
{
public:
  CSOcket(std::string host, std::function<void(std::string, CSOcket *)> onmessage);
  std::string host;
  uint16_t port = 8000;
  void connect();
  void step();
  void write(std::string);
  std::function<void(std::string, CSOcket *)> onmessage;
  bool connection = false;
  WiFiClient cli;
  std::stringstream receiveBuffer;
};

#endif
