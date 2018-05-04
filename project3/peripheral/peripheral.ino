#include "uno.h"
#define START 9
#include <Time.h>
char inbyte;
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 75;
int inPin = 7;
int val = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(inPin, INPUT);
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
	  if ((funcIndex & 0b100)) {
        //MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        //measure(&data, 3);
        
        unsigned long start = micros();
        int cur = 0;
        int prev = 0;
        int count = 0;
        while ((micros()) - start < (unsigned long)1000) {
          cur = digitalRead(inPin);     // read the input pin
          if (prev == 0 && cur == 1) {
            count = count + 1;
          }
          prev = cur;
        }
        pulseRateRaw = count;   
    }

    
	  if ((funcIndex & 0b010)) {
        MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw};
        measure(&data, 3);
    }

    Serial.write(9);
	  Serial.write((char)temperatureRaw);
	  Serial.write((char)systolicPressRaw);
    /*
    Serial.println("systo");
    Serial.println(systolicPressRaw);
	  */
	  Serial.write((char)diastolicPressRaw);
    /*
    Serial.println("diasto");
    Serial.println(diastolicPressRaw);
	  */
	  Serial.write((char)pulseRateRaw);
	  Serial.write(0);
   
  }
}


