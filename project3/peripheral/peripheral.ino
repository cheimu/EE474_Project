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
  while (Serial.available() < 3 );
  inbyte = Serial.read();
  if (inbyte == (char)9) {
       char funcIndex = Serial.read(); // s = 3'bxxx
       char endByte = Serial.read();

      if ((funcIndex & 0b001)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        measure(&data, 2);
        
      }
	  if ((funcIndex & 0b010)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        measure(&data, 3);
        
      }
	  if ((funcIndex & 0b100)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        measure(&data, 4);
 
      }

    
    Serial.write(9);
	  Serial.write((char)temperatureRaw);
	  Serial.write((char)systolicPressRaw);
	  Serial.write((char)diastolicPressRaw);
	  Serial.write((char)pulseRateRaw);
	  Serial.write(0);
  }
}


