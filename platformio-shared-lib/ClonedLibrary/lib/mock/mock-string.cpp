#include "mock-string.h"
#include <string.h>
#include <string>
#include <stdlib.h>
String::String(const char *cstr)
{
    cstr_val = cstr;
    len = strlen(cstr);
}
String::String(char cstr)
{
    cstr_val = std::string(1, cstr);
    len = cstr_val.length();
}
bool isspace(char charP)
{
    return charP == 32;
}

void String::trim()
{
    /*if (!cstr_val || len == 0)
        return;
    char *begin = cstr_val;
    while (isspace(*begin))
        begin++;
    char *end = cstr_val + len - 1;
    while (isspace(*end) && end >= begin)
        end--;
    unsigned int newlen = end + 1 - begin;

    char *ptr = "";
    for (int i = begin, j = 0; i < end; i++, j++)
    {
        output[j] = output[i];
    }
    if (begin > cstr_val)
        memmove(wbuffer(), begin, newlen);
    setLen(newlen);
    wbuffer()[newlen] = 0;*/
}

bool String::startsWith(String other)
{
    std::string currentStr = cstr_val;
    std::string startStr = other.c_str();

    return currentStr.rfind(startStr, 0) == 0;
}

bool String::endsWith(String other)
{
    std::string currentStr = cstr_val;
    std::string ending = other.c_str();

    if (ending.size() > currentStr.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), currentStr.rbegin());
}

int String::length()
{
    return len;
}

String String::substring(int start, int end)
{
    std::string str = cstr_val;
    std::string small = str.substr(start, end - start);
    return String(small.c_str());
}
bool String::operator<(const String &rhs) const
{
    std::string currentStr = cstr_val;
    std::string other = rhs.cstr_val;

    return currentStr < other;
}
unsigned char String::operator==(const String &rhs) const
{
    std::string currentStr = cstr_val;
    std::string other = rhs.cstr_val;

    return currentStr == other;
}
unsigned char String::operator==(const char *cstr) const
{
    std::string currentStr = cstr_val;
    std::string other = cstr;

    return currentStr == other;
}
void String::operator+=(const String rhs)
{
    std::string currentStr = cstr_val;
    std::string other = rhs.cstr_val;

    cstr_val = currentStr + other;

    len = cstr_val.length();
}

String String::operator+(const String rhs)
{

    // char *currentPt;
    // strncpy(currentPt, cstr_val, sizeof(cstr_val));
    std::string currentStr = cstr_val;
    std::string other = rhs.cstr_val;

    std::string joined = currentStr + other;

    return String(joined.c_str());
}

const char *String::c_str()
{
    return cstr_val.c_str();
}

char String::operator[](int index)
{
    return cstr_val.at(index * sizeof(char));
}
/*
std::stringstream &operator<<(std::stringstream &os, const String &dt)
{
    std::string currentStr = dt.cstr_val;
    const char[] str = currentStr.c_str();
    os << str;
    return os;
}*/

void String::replace(String from, String to)
{
    std::string str = cstr_val;

    size_t start_pos = str.find(from.cstr_val);
    if (start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to.c_str());
    cstr_val = str;
    len = cstr_val.length();
}

std::size_t String::indexOf(String c)
{
    std::string str = cstr_val;
    size_t start_pos = str.find(c.c_str());
    if (start_pos == std::string::npos)
        return -1;
    return start_pos;
}