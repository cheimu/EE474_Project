#include "uno.h"
#define START 9
#include <Time.h>


void setup()
{
  Serial.begin(9600);
  pinMode(tempPin, INPUT);
  pinMode(bpPin, INPUT);
  pinMode(prPin, INPUT);
  pinMode(rrPin, INPUT);
}


void loop()
{
  while (Serial.available() < 3 );
  inbyte = Serial.read();
  
  if (inbyte == (char)9) {
       char funcIndex = Serial.read(); // s = 3'bxxx
       char endByte = Serial.read();

      // temperature
      if ((funcIndex & 0b001)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
        measure(&data, 1);
        
        
      }

      // blood pressure
	  if ((funcIndex & 0b100)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
        measure(&data, 2);
        
      
    }

    // pulse rate
	  if ((funcIndex & 0b010)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
        measure(&data, 3);
       
    }

    // respiration rate
    if ((funcIndex & 0b1000)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
        measure(&data, 4);
       
    }

    Serial.write(9);
	  Serial.write((char)temperatureRaw);
	  Serial.write((char)systolicPressRaw);
	  Serial.write((char)diastolicPressRaw);
	  Serial.write((char)pulseRateRaw);
    Serial.write((char)respirationRateRaw);
	  Serial.write(0);
   
  }
}


