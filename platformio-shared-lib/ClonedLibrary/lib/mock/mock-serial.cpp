#include "mock-serial.h"
#include <iostream>

void MockSerial::println(const char *str)
{
    std::cout << str << "\n";
};
MockSerial::MockSerial(){};

MockSerial Serial;