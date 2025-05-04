#ifndef JSON_NODE
#define JSON_NODE

#include "arduinoref.h"
#include <map>
#include <vector>

class JsonNode
{
public:
  JsonNode();
  JsonNode(std::string strValue);
  JsonNode(const char *strValue);
  JsonNode(int intValue);

  std::string textValue;
  bool isText = false;
  bool isNumber = false;
  bool isObject = false;
  bool isArray = false;
  bool isBool = false;
  bool isNull = false;

  bool wasSet()
  {
    return isText || isNull || isNumber || isObject || isBool || isArray;
  }

  std::map<std::string, JsonNode> objectContent;
  std::string *objectKeys();
  std::vector<JsonNode> arrayContent;

  int numberValue = 0;
  bool boolValue;
  std::string valueCollector;
  std::string keyValue;

private:
};

JsonNode parseJson(String content);
std::string stringify(JsonNode node);

class JsonFactory
{
public:
  static JsonNode obj(std::map<std::string, JsonNode> mapValue)
  {
    JsonNode node;
    node.isObject = true;

    for (std::map<std::string, JsonNode>::iterator it = mapValue.begin(); it != mapValue.end(); ++it)
    {
      std::pair<std::string, JsonNode> pair(it->first, it->second);
      node.objectContent.insert(pair);
    }
    return node;
  }

  static JsonNode list(std::vector<JsonNode> arrayValue)
  {
    JsonNode node;
    node.isArray = true;

    for (std::vector<JsonNode>::iterator it = arrayValue.begin(); it != arrayValue.end(); ++it)
    {
      node.arrayContent.push_back(*it);
    }
    return node;
  }

  static JsonNode TRUE()
  {
    JsonNode node;
    node.isBool = true;
    node.boolValue = true;

    return node;
  }

  static JsonNode FALSE()
  {
    JsonNode node;
    node.isBool = true;
    node.boolValue = false;
    return node;
  }

  static JsonNode str(std::string strVal)
  {
    JsonNode node(strVal);
    return node;
  }

  static JsonNode num(int intVal)
  {
    JsonNode node(intVal);
    return node;
  }
};

#endif