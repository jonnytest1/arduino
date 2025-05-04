#ifndef SMARTHOME
#define SMARTHOME

#include "http.h"
#include "jsonnode.h"
#include <vector>

void setPinHighTimed(
    int pin, int millis, std::function<void()> ondone = NULL, int state = HIGH);

class SmartHome
{
public:
  SmartHome(std::function<void(std::string, SmartHome *)> callback);

  String operator()(HttpRequest *client)
  {
    return onRequest(client);
  }

  std::function<JsonNode()> getReceiverConfig = nullptr;
  std::function<JsonNode()> getSenderConfig = nullptr;
  std::function<std::string(HttpRequest *)> fallbackRequestHandler = nullptr;

  void init();
  void ota();
  void step();
  void updateState(JsonNode state);
  void next(std::function<void()> callback);
  void registerAction(std::string name, std::function<void(SmartHome *)> callback, bool confirm = false);
  void triggerSenderEvent(std::string name, JsonNode data);

  unsigned long delta_millis;
  unsigned long last_loop_millis;
  int stepCt = 0;

private:
  HttpServer server;
  String onRequest(HttpRequest *client);
  std::function<void(std::string, SmartHome *)> actionCallback;

  void stepPendingActions();
  void triggerAction(std::string name, SmartHome *sm);
  void registerReceiver();
  void registerSender();
  bool hasRegisteredReceiver;
  bool hasRegisteredSender;
  bool otaEnabled;
};

#endif