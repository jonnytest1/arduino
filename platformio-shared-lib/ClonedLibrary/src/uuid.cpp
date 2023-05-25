#include "arduinoref.h"
using namespace std::__cxx11;

char const *letters = "abcdefghijklmnopqrstuvwxyz0123456789";

char randomLetter()
{
    byte randomValue = random(0, 36);
    char letter = randomValue + 'a';
    if (randomValue >= 26)
        letter = (randomValue - 26) + '0';
    return letter;
}

string generateUuid()
{
    string uuidAcc = "";

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            uuidAcc += randomLetter();
        }
        if (i != 3)
        {
            uuidAcc += "-";
        }
    }

    return uuidAcc;
}
