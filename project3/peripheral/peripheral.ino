#include "uno.h"
#define START 9
char inbyte;
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 75;



void setup()
{
  Serial.begin(9600);

}


void loop()
{
  while (Serial.available() <4 );
  inbyte = Serial.read();
  if (inbyte == (char)9) {
       char funcIndex = Serial.read();
        char which = Serial.read();
        char endByte = Serial.read();

      if (funcIndex == 1) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        measure(&data, which);
        
        Serial.write(9);
        Serial.write(funcIndex);
        Serial.write((char)temperatureRaw);
        Serial.write(0);
        //Serial.println((int)temperatureRaw);
      }
      
  
  }
}


