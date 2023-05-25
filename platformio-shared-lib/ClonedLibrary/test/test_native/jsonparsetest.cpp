#include <unity.h>
#include <jsonnode.h>
#include <str.h>
#include <exception>
#include <stdexcept>

void setUp()
{
}

void tearDown()
{
}

void testParseExample1()
{
    String test_json = "{\n    \"test\":123,\n    \"abc\":[\"test\",\"123\"]\n}";
    auto tnode = parseJson(test_json);
    TEST_ASSERT(tnode.isObject);
    TEST_ASSERT(tnode.objectContent.at("test").isNumber);
    TEST_ASSERT(tnode.objectContent.at("abc").isArray);
    printf("done parse ex1");
}
void testParseNumber()
{
    JsonNode nodeNum = parseJson("123");

    TEST_ASSERT(nodeNum.isNumber);
    TEST_ASSERT(nodeNum.numberValue == 123);
}

void testParseStr()
{
    JsonNode nodeTxt = parseJson("\"123\"");

    TEST_ASSERT(nodeTxt.isText);
    TEST_ASSERT(nodeTxt.textValue == "123");
}
void testParseBool()
{
    JsonNode nodeBool = parseJson("true");

    TEST_ASSERT(nodeBool.isBool);
    TEST_ASSERT(nodeBool.boolValue);
    JsonNode nodeBoolF = parseJson("false");

    TEST_ASSERT(nodeBoolF.isBool);
    TEST_ASSERT(nodeBoolF.boolValue == false);
}
void testParseArray()
{
    JsonNode nodeBoolAr = parseJson("[true,false]");

    TEST_ASSERT(nodeBoolAr.isArray);
    TEST_ASSERT(nodeBoolAr.arrayContent.size() == 2);
    JsonNode firstItem = nodeBoolAr.arrayContent.at(0);
    JsonNode secondItem = nodeBoolAr.arrayContent.at(1);
    TEST_ASSERT(firstItem.isBool == true);
    TEST_ASSERT(firstItem.boolValue == true);
    TEST_ASSERT(secondItem.isBool == true);
    TEST_ASSERT(secondItem.boolValue == false);
}
void testParseObject()
{
    JsonNode nodeObj = parseJson("{\"test\":123}");
    TEST_ASSERT(nodeObj.isObject == true);
    int size = nodeObj.objectContent.size();
    TEST_ASSERT(size == 1);
    for (std::map<std::string, JsonNode>::iterator it = nodeObj.objectContent.begin(); it != nodeObj.objectContent.end(); ++it)
    {
        TEST_ASSERT(it->first == "test");
    }
    JsonNode firstItemNst = nodeObj.objectContent.at("test");
    TEST_ASSERT(firstItemNst.isNumber == true);
    TEST_ASSERT(firstItemNst.numberValue == 123);
}

void testParse()
{
    testParseNumber();
    testParseStr();
    testParseBool();
    testParseArray();
    testParseObject();

    JsonNode objWithSpace = parseJson("{\n    \"signal\":\"LSSSLSSSSLLSLSSLSSSSLLLSS\",\n    \"baud\":2466\n}\n");
    TEST_ASSERT(objWithSpace.isObject == true);
    int sizeObjSpace = objWithSpace.objectContent.size();
    TEST_ASSERT(sizeObjSpace == 2);
    JsonNode objWithSpaceSignal = objWithSpace.objectContent.at("signal");
    JsonNode objWithSpaceBaud = objWithSpace.objectContent.at("baud");
    TEST_ASSERT(objWithSpaceSignal.isText);
    TEST_ASSERT(objWithSpaceBaud.isNumber);
    testParseExample1();
    printf("done parse");
}

void testStringify()
{
    auto str = stringify(JsonFactory::TRUE());
    TEST_ASSERT(str == "true");
    auto strF = stringify(JsonFactory::FALSE());
    TEST_ASSERT(strF == "false");

    auto strN = stringify(123);
    TEST_ASSERT(strN == "123");

    auto strT = stringify("test");
    TEST_ASSERT(strT == "\"test\"");

    auto strO = stringify(JsonFactory::obj({{"test", 123}}));
    TEST_ASSERT(strO == "{\"test\":123}");

    JsonNode node = JsonFactory::list({"test", "abc"});
    TEST_ASSERT(stringify(node) == "[\"test\",\"abc\"]");

    auto strNest = stringify( //
        JsonFactory::obj({    //
                          {"test", 123},
                          {"abc", JsonFactory::list({
                                      //
                                      JsonFactory::obj({{"inner", 123}}) //
                                  })}}));
    TEST_ASSERT(strNest == "{\"abc\":[{\"inner\":123}],\"test\":123}");

    TEST_ASSERT(stringify(JsonFactory::str("test\"")) == "\"test\\\"\"");

    std::map<std::string, JsonNode> data;

    std::string json_string = stringify(JsonFactory::str("test"));
    data.insert(std::pair<std::string, JsonNode>("message", JsonFactory::str(json_string)));
    std::string nested_json = stringify(JsonFactory::obj(data));

    TEST_ASSERT(nested_json == "{\"message\":\"\\\"test\\\"\"}");
}

void run()
{
    String jsonStr = "123";
    String second = "456";
    String combined = jsonStr + second;

    TEST_ASSERT(combined == "123456");

    combined += second;
    TEST_ASSERT(combined == "123456456");
    try
    {
        testParse();
        testStringify();
    }
    catch (const std::exception &ex)
    {
        printf("ex exceptiomn");
        printf(ex.what());
    }
    catch (const std::string &ex)
    {
        printf("str exceptiomn");
        printf(ex.c_str());
    }
    catch (...)
    {
        printf("unknown exceptiomn");
    }
    // TEST_ASSERT(1 == 2);

    printf("done");
}

void runBTest()
{

    uint8_t bufferPwr[] = {
        0b11101000, // long short
        0b10000000, // short short
    };

    char longHigh[] = "1110";
    char shortHigh[] = "1000";

    char byteStr[] = "LSS";
    std::string byteString = byteStr;

    str_replace_all(byteString, "S", shortHigh);
    str_replace_all(byteString, "L", longHigh);

    double size = byteString.length();
    int bufferSize = ceil(size / 8);

    uint8_t customBuffer[bufferSize];

    for (int i = 0; i < bufferSize; i++)
    {
        std::string substr = sub_string(byteString, i * 8, (i * 8) + 8);
        int num = strtol(substr.c_str(), nullptr, 2);

        if (substr.length() == 4)
        {
            num << 4;
        }

        customBuffer[i] = num;
    }

    TEST_ASSERT(customBuffer[0] == bufferPwr[0]);
    TEST_ASSERT(customBuffer[1] == bufferPwr[1]);
}
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(run);
    RUN_TEST(runBTest);

    UNITY_END();
}
