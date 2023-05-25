#include "smarthome.h"
#include "http.h"
#include "jsonnode.h"
#include "http_client.h"
#include "log.h"
#include "prop.h"
#include <list>

struct PendingAction
{
    std::string name;
    int remaining_millis = 10 * 1000;
    std::function<void()> callback;
};

struct ActionConfig
{
    bool confirm;
};

std::list<PendingAction> pendingActions;

std::map<std::string, ActionConfig> actionConfig;

void setPinHighTimed(int pin, int millis, int state)
{
    digitalWrite(pin, state);

    PendingAction action;
    action.callback = [pin, state]()
    {
        digitalWrite(pin, 1 - state);
    };
    action.name = "reset pin";

    pendingActions.push_back(action);
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
                    actionCallback(name.textValue, this);
                    pendingActions.erase(it);
                    break;
                }
            }
        }
        else if (actionConfig.find(name.textValue) == actionConfig.end())
        {
            logData("ERROR", "didnt find key in actionConfig", {{"key", name.textValue}});
        }
        else if (actionConfig.at(name.textValue).confirm)
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
            actionCallback(name.textValue, this);
        }
    }
    else
    {
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
void SmartHome::registerReceiver()

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

    HttpClientRequest request(serverEndpoint() + "/nodets/rest/receiver");
    request.jsonBody(stringify(node));
    request.callback = negativeResponseLogger;
    Serial.println("sending reuqest");
    request.send();
};

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
            Serial.print(secondsA);
            Serial.print(" remaining for action: ");
            Serial.println(it->name.c_str());
        }
        if (it->remaining_millis < 0)
        {
            Serial.println("triggering timed out");
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

void SmartHome::step()

{
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
    server.step();

    stepPendingActions();

    delta_millis = millis() - last_loop_millis;
    last_loop_millis = millis();
    stepCt++;
};

void SmartHome::updateState(JsonNode state)
{
    Serial.println("update state");
    HttpClientRequest request(serverEndpoint() + "/nodets/rest/receiver/state");
    request.jsonBody(stringify(JsonFactory::obj({
        {"deviceKey", JsonFactory::str(getDeviceKey().c_str())},
        {"state", state},
    })));
    request.callback = negativeResponseLogger;
    request.send();
}