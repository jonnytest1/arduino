#include <iostream>

class String
{
public:
    String(const char *cstr = "");
    explicit String(const char cstr);
    char operator[](int index);
    void trim();
    bool startsWith(String other);
    bool endsWith(String other);
    int length();
    String substring(int start, int end);
    void replace(String start, String end);
    std::size_t indexOf(String c);
    unsigned char operator==(const String &rhs) const;
    unsigned char operator==(const char *cstr) const;
    bool operator<(const String &rhs) const;

    void operator+=(const String rhs);
    String operator+(const String rhs);
    const char *c_str();
    // friend std::stringstream &operator<<(std::stringstream &os, const String &dt);

private:
    std::string cstr_val;
    unsigned int len;
};