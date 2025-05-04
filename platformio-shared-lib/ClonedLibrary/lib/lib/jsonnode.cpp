#include "jsonnode.h"
#include "str.h"
#include <list>
#include "arduinoref.h"

void log(char *args)
{
}
/*
char *ErrorTypes[] =
    {
        "EXPECTING_JSON",
        "EXPECTING_STRING_END",
        "EXPECTING_NUMBER_END",
        "EXPECTING_ARRAY_END",
        "EXPECTING_OBJECT_KEY_START",
        "EXPECTING_OBJECT_KEY_END",
        "EXPECTING_OBJECT_KEY_COLON",
        "EXPECTING_OBJECT_END",
        "DONE"};*/
enum ParseModes
{
  EXPECTING_JSON,
  EXPECTING_STRING_END,
  EXPECTING_NUMBER_END,
  EXPECTING_ARRAY_END,
  EXPECTING_OBJECT_KEY_START,
  EXPECTING_OBJECT_KEY_END,
  EXPECTING_OBJECT_KEY_COLON,
  EXPECTING_OBJECT_END,
  DONE
};
JsonNode::JsonNode()
{
}

JsonNode::JsonNode(std::string strVal)
{
  isText = true;
  textValue = strVal;
}
JsonNode::JsonNode(const char *strVal)
{
  isText = true;
  std::string textV = strVal;
  textValue = textV;
}
JsonNode::JsonNode(int num)
{
  isNumber = true;
  numberValue = num;
}

JsonNode parseJson(String body)
{
  std::list<ParseModes> parseModes = {EXPECTING_JSON, DONE};

  parseModes.front();
  JsonNode initial;
  std::list<JsonNode *> nodes = {&initial};
  int size = body.length();
  for (int i = 0; i < size; i++)
  {
    /* auto logModes = [&parseModes, &body, i]()
     {
         auto it = parseModes.begin();

         printf(body.substring(0, i).c_str());
         printf("\n");
         while (it != parseModes.end())
         {
             ParseModes p = *it;
             int index = p;
             char *pType = ErrorTypes[index];
             printf(pType);
             printf(" ");
             ++it;
         }
         printf("\n");
     };*/

    char character = body[i];

    ParseModes mode = parseModes.front();
    JsonNode *current = nodes.front();

    if (mode == EXPECTING_JSON)
    {
      if (character == '"')
      {
        current->isText = true;
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_STRING_END);
      }
      else if (isNumeric(String(character)))
      {
        current->isNumber = true;
        current->valueCollector += character;
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_NUMBER_END);
      }
      else if (character == 't' && body[i + 1] == 'r' && body[i + 2] == 'u' && body[i + 3] == 'e')
      {
        current->isBool = true;
        current->boolValue = true;
        parseModes.pop_front();
        i += 3;
      }
      else if (character == 'f' && body[i + 1] == 'a' && body[i + 2] == 'l' && body[i + 3] == 's' && body[i + 4] == 'e')
      {
        current->isBool = true;
        current->boolValue = false;
        parseModes.pop_front();
        i += 4;
      }
      else if (character == '[')
      {

        JsonNode *itemNode = new JsonNode();
        current->isArray = true;
        nodes.push_front(itemNode);
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_ARRAY_END);
        parseModes.push_front(EXPECTING_JSON);
        // logModes();
      }
      else if (character == '{')
      {

        current->isObject = true;
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_OBJECT_KEY_START);
        // logModes();
      }
      else
      {
        // new json type ?
      }
    }
    else if (mode == EXPECTING_NUMBER_END)
    {
      if (!isNumeric(String(character)))
      {
        current->numberValue = stoi(current->valueCollector.c_str());
        parseModes.pop_front();
        mode = parseModes.front();

        // printf("TODO");
      }
      else
      {
        current->valueCollector += character;
      }
    }

    if (mode == EXPECTING_STRING_END)
    {
      if (character == '"')
      {
        current->textValue = current->valueCollector;
        parseModes.pop_front();
      }
      else
      {
        current->valueCollector += character;
      }
    }
    else if (mode == EXPECTING_ARRAY_END)
    {
      if (character == ',')
      {
        JsonNode prevItem = *nodes.front();
        if (!prevItem.wasSet())
        {
          throw std::runtime_error("item wasnt set but found ,");
        }
        nodes.pop_front();
        JsonNode *arrayParent = nodes.front();

        arrayParent->arrayContent.push_back(prevItem);

        JsonNode *newNode = new JsonNode();

        nodes.push_front(newNode);
        parseModes.push_front(EXPECTING_JSON);
        // logModes();
      }
      else if (character == ']')
      {
        JsonNode prevItem = *nodes.front();
        // printf("array end:\n");
        // logModes();
        if (prevItem.wasSet())
        {
          nodes.pop_front();
          JsonNode *arrayParent = nodes.front();

          arrayParent->arrayContent.push_back(prevItem);
        }

        parseModes.pop_front();
        if (parseModes.front() != EXPECTING_OBJECT_END)
        {
          nodes.pop_front();
        }
      }
    }
    else if (mode == EXPECTING_OBJECT_KEY_START)
    {
      if (character == '"')
      {

        parseModes.pop_front();
        parseModes.push_front(EXPECTING_OBJECT_KEY_END);
      }
      // ignore whitespace
    }
    else if (mode == EXPECTING_OBJECT_KEY_END)
    {
      if (character == '"')
      {

        JsonNode *keyData = new JsonNode();
        keyData->keyValue = current->valueCollector.c_str();

        nodes.push_front(keyData);
        parseModes.push_front(EXPECTING_OBJECT_KEY_COLON);
        // logModes();
      }
      else
      {
        current->valueCollector += character;
      }
    }
    else if (mode == EXPECTING_OBJECT_END)
    {
      if (character == ',')
      {
        // logModes();
        JsonNode *item = nodes.front();
        nodes.pop_front();
        JsonNode *objectParent = nodes.front();
        if (!item->wasSet())
        {
          throw std::runtime_error("object item wasnt set but found ,");
        }
        JsonNode itemVal = *item;
        std::string key = itemVal.keyValue;
        if (key.length() == 0)
        {
          throw std::runtime_error("object key wasnt set at comma");
        }
        std::pair<std::string, JsonNode>
            pair(key, itemVal);
        objectParent->objectContent.insert(pair);
        objectParent->valueCollector = "";
        // printf("object comma:\n");
        // logModes();
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_OBJECT_KEY_START);
      }
      else if (character == '}')
      {
        // logModes();
        JsonNode *item = nodes.front();
        nodes.pop_front();
        JsonNode *objectParent = nodes.front();
        if (item->wasSet())
        {
          JsonNode currentItem = *item;

          std::string key = currentItem.keyValue;

          if (key.length() == 0)
          {
            throw std::runtime_error("object key wasnt set");
          }
          std::pair<std::string, JsonNode> pair(key, currentItem);

          objectParent->objectContent.insert(pair);
        }
        // printf("object end:\n");
        // logModes();
        parseModes.pop_front();
      }
    }
    else if (mode == EXPECTING_OBJECT_KEY_COLON)
    {
      if (character == ':')
      {
        parseModes.pop_front();
        parseModes.push_front(EXPECTING_OBJECT_END);
        parseModes.push_front(EXPECTING_JSON);
      }
    }

    if (i == size - 1)
    {

      ParseModes finalMode = parseModes.front();
      if (finalMode == EXPECTING_NUMBER_END)
      {
        current->numberValue = stoi(current->valueCollector.c_str());
        parseModes.pop_front();
      }
      else if (finalMode == DONE)
      {
        // nothing to do
      }
      else
      {
        // printf("last");
      }
    }
  }
  ParseModes finalMode = parseModes.front();
  if (finalMode != DONE)
  {
    // printf("not done");
  }
  return initial;
}

std::string *JsonNode::objectKeys()
{

  std::string *keyAr = new std::string[objectContent.size()];
  int ct = 0;
  for (std::map<std::string, JsonNode>::iterator it = objectContent.begin(); it != objectContent.end(); ++it)
  {
    std::string key = it->first;
    const char *keyVal = key.c_str();
    keyAr[ct++] = keyVal;
  }

  return keyAr;
}

std::string stringify(JsonNode node)
{
  if (node.isBool)
  {
    if (node.boolValue)
    {
      return "true";
    }
    else
    {
      return "false";
    }
  }
  else if (node.isNumber)
  {
    return itos(node.numberValue);
  }
  else if (node.isText)
  {
    std::string cpy = node.textValue;
    std::string srepl = str_replace_all(cpy, "\\", "\\\\");
    std::string qrepl = str_replace_all(srepl, "\"", "\\\"");
    std::string nrepl = str_replace_all(qrepl, "\n", "\\n");
    std::string trepl = str_replace_all(nrepl, "\t", "\\t");
    std::string rrepl = str_replace_all(trepl, "\r", "\\r");

    return "\"" + rrepl + "\"";
  }
  else if (node.isObject)
  {
    std::string collector = "{";

    bool first = true;
    for (std::map<std::string, JsonNode>::iterator it = node.objectContent.begin(); it != node.objectContent.end(); ++it)
    {
      if (!first)
      {
        collector += ",";
      }
      first = false;
      std::string key = it->first;
      JsonNode keyNode = JsonNode(key.c_str());
      std::string keyStrJSson = stringify(keyNode);
      std::string valueStr = stringify(it->second);
      collector += keyStrJSson + ":" + valueStr;
    }
    collector += "}";
    return collector;
  }
  else if (node.isArray)
  {
    std::string collector = "[";

    bool first = true;
    for (std::vector<JsonNode>::iterator it = node.arrayContent.begin(); it != node.arrayContent.end(); ++it)
    {
      if (!first)
      {
        collector += ",";
      }
      first = false;
      JsonNode obj = *it;
      std::string keyStrJSson = stringify(obj);
      collector += keyStrJSson;
    }
    collector += "]";
    return collector;
  }
  else
  {
    Serial.println("got not implemented type");
    throw "not implemented";
  }
}