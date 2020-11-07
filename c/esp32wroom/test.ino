#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
}

void loop()
{
    static uint32_t oldtime = millis();
    // put your main code here, to run repeatedly:
    double curvolt = double(readVcc()) / 1000;
    if ((millis() - oldtime) > (5000))
    {
        oldtime = millis();
        Serial.write("current volatage ");
        Serial.println(String(dtoa(curvolt, 4)));
    }
}

char *dtoa(double dN, int iP)
{
    char cVal[10];
    char *cMJA = cVal;
    char *ret = cMJA;
    long lP = 1;
    byte bW = iP;
    while (bW > 0)
    {
        lP = lP * 10;
        bW--;
    }
    long lL = long(dN);
    double dD = (dN - double(lL)) * double(lP);
    if (dN >= 0)
    {
        dD = (dD + 0.5);
    }
    else
    {
        dD = (dD - 0.5);
    }
    long lR = abs(long(dD));
    lL = abs(lL);
    if (lR == lP)
    {
        lL = lL + 1;
        lR = 0;
    }
    if ((dN < 0) & ((lR + lL) > 0))
    {
        *cMJA++ = '-';
    }
    ltoa(lL, cMJA, 10);
    if (iP > 0)
    {
        while (*cMJA != '\0')
        {
            cMJA++;
        }
        *cMJA++ = '.';
        lP = 10;
        while (iP > 1)
        {
            if (lR < lP)
            {
                *cMJA = '0';
                cMJA++;
            }
            lP = lP * 10;
            iP--;
        }
        ltoa(lR, cMJA, 10);
    }
    return ret;
}
//read internal voltage
long readVcc()
{
    long result;
    // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);            // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; // Back-calculate AVcc in mV
    return result;
}