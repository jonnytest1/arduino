void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);  
    Serial.println("init");
}

int incomingByte = 0;

void loop()
{
  

    incomingByte = Serial.read();
    if(incomingByte!=-1){
      
        // say what you got:
        //Serial.print(incomingByte);
        //Serial.print((byte) incomingByte);  // print the byte we received as a decimal value
        //Serial.print(" --> ");
        Serial.print((char) incomingByte);
    }
}
