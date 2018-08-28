#include "uno.h"
#define START 9
#include <Time.h>




void setup()
{
  Serial.begin(9600);
  pinMode(tempPin, INPUT);
  pinMode(swich, INPUT);
  pinMode(bpPin, INPUT);
  pinMode(prPin, INPUT);
  pinMode(button,INPUT);
  pinMode(oPin,OUTPUT);
  pinMode(interPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interPin), cuff, RISING);
  attachInterrupt(digitalPinToInterrupt(prPin), pr_detect, RISING);
  
}


void loop()
{
  debounce(&swichState, &lastSwichState, &isUp, &lastDebounceTime, &debounceDelay, swich);
  button_debounce(&buttonState, &lastButtonState, &isButtonUp, &b_lastDebounceTime, &b_debounceDelay, button);
  //Serial.println("Cuff state: ");
  //Serial.println(Pressure);
  //Serial.println("is UP ");
  //Serial.println(isUp);
  //Serial.println("act pressure");
  //Serial.println(systolicPressRaw);
  //Serial.println(diastolicPressRaw);
  
  if  (Serial.available() >= 3 ) {
  
    inbyte = Serial.read();

   if (inbyte == (char)9) {

       functionIndex = Serial.read(); // s = 4'bxxxx
       endByte = Serial.read();

       
       if (functionIndex & 0b0010) {
         attachInterrupt(digitalPinToInterrupt(prPin), pr_detect, RISING);
         detachInterrupt(prPin);
       }


      // temperature
        if (functionIndex & 0b0001) {
          MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
          measure(&data, 1);
        }
        


  

    

    

        // blood pressure
        if (functionIndex & 0b0100) {
          MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
          measure(&data, 3);
        }
      
    

    // respiration rate
        //if (functionIndex & 0b1000) {
         // MeasureData data = {&temperatureRaw, &systolicPressRaw, &diastolicPressRaw, &pulseRateRaw, &respirationRateRaw};
         // measure(&data, 4);
        //} 
    
  
  

      Serial.write(9);
  	  Serial.write((char) temperatureRaw);
      Serial.write((char) Pressure); 
  	  Serial.write((char) systolicPressRaw);
  	  Serial.write((char) diastolicPressRaw);
  	  Serial.write((char) pulseRateRaw);
      Serial.write((char) respirationRateRaw);
  	  Serial.write(0);
   }
  }
}


