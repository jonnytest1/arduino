#include "smarthome.h"
#include "http.h"
#include "jsonnode.h"
#include "http_client.h"
#include "log.h"
#include "prop.h"
#include <list>
#include <ArduinoOTA.h>
#include "uuid.h"
#include "wi_fi.h"

struct PendingAction
{
  std::string name;
  int remaining_millis = 10 * 1000;
  std::function<void()> callback;
};

struct ActionConfig
{
  bool confirm;
  std::function<void(SmartHome *)> callback = nullptr;
};

std::list<PendingAction> pendingActions;

std::map<std::string, ActionConfig> actionConfig;

void setPinHighTimed(
    int pin, int millis, std::function<void()> ondone, int state)
{
  digitalWrite(pin, state);

  PendingAction action;

  action.callback = [pin, state, ondone]()
  {
    digitalWrite(pin, 1 - state);

    if (ondone)
    {
      Serial.print("executing callback");
      ondone();
    }
  };

  action.name = "reset pin";
  action.remaining_millis = millis;

  pendingActions.push_back(action);
}

void SmartHome::triggerAction(std::string name, SmartHome *sm)
{
  if (actionConfig.find(name) == actionConfig.end())
  {
    logData("ERROR", "didnt find key in actionConfig", {{"key", name}});
    return;
  }
  ActionConfig cfg = actionConfig.at(name);
  if (cfg.callback != nullptr)
  {
    cfg.callback(this);
  }
  else
  {
    actionCallback(name, this);
  }
}

String SmartHome::onRequest(HttpRequest *client)
{
  if (client->path.indexOf("action") > -1)
  {

    JsonNode body = parseJson(client->body);
    Serial.println("parsing done");
    JsonNode name = body.objectContent.at("name");

    if (client->path.indexOf("confirm") > -1)
    {
      std::list<PendingAction>::iterator it = pendingActions.begin();
      while (it != pendingActions.end())
      {
        if (it->name == name.textValue)
        {
          triggerAction(name.textValue, this);
          pendingActions.erase(it);
          break;
        }
      }
    }
    else if (actionConfig.find(name.textValue) == actionConfig.end())
    {
      logData("ERROR", "didnt find key in actionConfig", {{"key", name.textValue}});
      client->responseStatus = 404;
      return "didnt find action";
    }
    ActionConfig cfg = actionConfig.at(name.textValue);

    if (cfg.confirm)
    {

      PendingAction action;
      action.callback = []() {

      };
      action.name = name.textValue;
      pendingActions.push_back(action);
      return "pending_confirmation";
    }
    else
    {
      triggerAction(name.textValue, this);
    }
  }
  else
  {
    if (fallbackRequestHandler == nullptr)
    {
      client->responseStatus = 404;
      return "handler not found";
    }
    return fallbackRequestHandler(client).c_str();
  }

  return "done";
};
SmartHome::SmartHome(std::function<void(std::string, SmartHome *)> actionCallbackRef)
    : server(80, [this](HttpRequest *client)
             { return onRequest(client); })
{
  actionCallback = actionCallbackRef;
};

void SmartHome::init()

{
  server.begin();
};

void SmartHome::ota()

{
  otaEnabled = true;
  std::string otaPassword = generateUuid();
  ArduinoOTA.setHostname(("esp32-" + getDeviceKey() + "_ota").c_str());
  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();
  logData("INFO", "startup log", {
                                     {"application", getDeviceKey().c_str()},
                                     {"otaPassword", otaPassword},
                                     {"hostname", getHostname().c_str()},
                                     {"ip", getDeviceIp().c_str()},
                                 });
};

void SmartHome::registerReceiver()

{
  if (getReceiverConfig != nullptr)
  {

    JsonNode node = getReceiverConfig();

    auto actions = node.objectContent.find("actions");
    if (actions != node.objectContent.end())
    {
      auto actionNode = actions->second;
      for (std::vector<JsonNode>::iterator it = actionNode.arrayContent.begin(); it != actionNode.arrayContent.end(); ++it)
      {
        auto name = it->objectContent.at("name").textValue;
        ActionConfig cfg = ActionConfig();
        auto confirmEl = it->objectContent.find("confirm");
        if (confirmEl != it->objectContent.end() && confirmEl->second.boolValue == true)
        {
          cfg.confirm = true;
        }
        std::pair<std::string, ActionConfig> cfgpair(name, cfg);
        actionConfig.insert(cfgpair);
      }
    }

    std::pair<std::string, JsonNode> pairDevKey("deviceKey", JsonFactory::str(getDeviceKey()));
    node.objectContent.insert(pairDevKey);

    std::pair<std::string, JsonNode> pairPort("port", JsonFactory::str(String(80).c_str()));
    node.objectContent.insert(pairPort);
    std::pair<std::string, JsonNode> pairType("type", JsonFactory::str("http"));
    node.objectContent.insert(pairType);

    std::string receiverUrl = serverEndpoint() + "/rest/receiver";
    HttpClientRequest request(receiverUrl);
    request.jsonBody(stringify(node));
    request.callback = negativeResponseLogger;
    Serial.println("sending reuqest");
    request.send();
  }
};

void SmartHome::registerAction(std::string name, std::function<void(SmartHome *)> actionCallbackRef, bool confirm)
{
  if (actionConfig.find(name) == actionConfig.end())
  {
    logData("ERROR", "didnt find key in actionConfig", {{"key", name}});
    return;
  }

  ActionConfig cfg = actionConfig.at(name);
  cfg.callback = actionCallbackRef;
}

void SmartHome::stepPendingActions()
{

  std::list<PendingAction>::iterator it = pendingActions.begin();
  while (it != pendingActions.end())
  {
    int delta = millis() - last_loop_millis;
    double millis = it->remaining_millis;
    double seconds = floor(millis / 1000);
    it->remaining_millis -= delta;
    double millisA = it->remaining_millis;
    double secondsA = floor(millisA / 1000);

    if (secondsA < seconds)
    {
      // Serial.print(secondsA);
      // Serial.print(" remaining for action: ");
      // Serial.println(it->name.c_str());
    }
    if (it->remaining_millis < 0)
    {
      Serial.println(String("triggering pending action ") + it->name.c_str());
      it->callback();
      pendingActions.erase(it);
      return;
    }
    else
    {
      ++it;
    }
  }
}
void SmartHome::next(std::function<void()> callback)
{
  PendingAction action;
  action.callback = callback;
  action.name = "next";
  action.remaining_millis = 0;

  pendingActions.push_back(action);
}

void SmartHome::triggerSenderEvent(std::string name, JsonNode data)
{
  std::pair<std::string, JsonNode> pairDevKey("deviceKey", JsonFactory::str(getDeviceKey()));
  data.objectContent.insert(pairDevKey);

  std::pair<std::string, JsonNode> messageKey("message", JsonFactory::str(name));
  data.objectContent.insert(messageKey);

  std::string receiverUrl = serverEndpoint() + "/rest/sender/trigger";
  HttpClientRequest request(receiverUrl);

  request.jsonBody(stringify(data));
  request.callback = negativeResponseLogger;
  Serial.println("sending reuqest");
  request.send();
}

void SmartHome::registerSender()
{

  if (getSenderConfig == nullptr)
  {
    return;
  }

  JsonNode node = getSenderConfig();
  std::pair<std::string, JsonNode> pairDevKey("deviceKey", JsonFactory::str(getDeviceKey()));
  node.objectContent.insert(pairDevKey);

  std::string receiverUrl = serverEndpoint() + "/rest/sender";
  HttpClientRequest request(receiverUrl);

  request.jsonBody(stringify(node));
  request.callback = negativeResponseLogger;
  Serial.println("sending reuqest");
  request.send();
}

void SmartHome::step()

{
  if (otaEnabled)
  {
    ArduinoOTA.handle();
  }

  if (stepCt % 100 == 0)
  {
    // Serial.print("free heap:");
    // Serial.println(esp_get_free_heap_size());
  }
  if (!hasRegisteredReceiver)
  {
    registerReceiver();
    hasRegisteredReceiver = true;
  }
  if (!hasRegisteredSender)
  {
    registerSender();
    hasRegisteredSender = true;
  }
  server.step();

  stepPendingActions();

  delta_millis = millis() - last_loop_millis;
  last_loop_millis = millis();
  stepCt++;
};

void SmartHome::updateState(JsonNode state)
{
  if (getReceiverConfig == nullptr)
  {
    return;
  }

  Serial.println("update state");
  HttpClientRequest request(serverEndpoint() + "/nodets/rest/receiver/state");
  request.jsonBody(stringify(JsonFactory::obj({
      {"deviceKey", JsonFactory::str(getDeviceKey())},
      {"state", state},
  })));
  request.callback = negativeResponseLogger;
  request.send();
}