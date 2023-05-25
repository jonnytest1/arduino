#ifndef CREDENTIALS
#define CREDENTIALS
#include <string>

char *ssid = "";
char *password = "";
char *serverHost = "";
char const *logHost = "";
char const *deviceKey = "";

std::string serverEndpoint();
std::string logEndpoint();
std::string getDeviceKey();

#endif
