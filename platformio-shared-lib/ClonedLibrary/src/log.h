#include "arduinoref.h"
#include <map>
#include "http_client.h"

void negativeResponseLogger(HttpClientRequest *request, int code, std::string data);
void logData(std::string logLevel, std::string message, const std::map<std::string, std::string> data);