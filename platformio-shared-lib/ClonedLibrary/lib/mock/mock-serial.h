#ifndef MOCKSER
#define MOCKSER
class MockSerial
{
public:
    MockSerial();
    void println(const char *str);
};

extern MockSerial Serial;

#endif