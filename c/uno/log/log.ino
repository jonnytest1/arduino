void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
}

int incomingByte = 0;

void loop()
{
    Serial.println("test2");

    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);

    delay(1000);
}