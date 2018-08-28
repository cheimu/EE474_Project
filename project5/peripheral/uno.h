#ifndef UNO
#define UNO



unsigned long global_timer = micros();
char inbyte;
char functionIndex;
char endByte;
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 75;
unsigned int respirationRateRaw =25;

int tempPin = A5;
int bpPin = A4;

int prPin = 2;

int swich = 9;
int button = 4;
int swichState;
int lastSwichState = 0;
int buttonState;
int lastButtonState = 0;
int counter = 0;
int isUp = 1;
int isButtonUp = 1;

int val = 0;
int interPin = 3;
int oPin = 11;
int Pressure = 70;

int pr_count = 0;

int EKGReady = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long b_lastDebounceTime = 0;
unsigned long b_debounceDelay = 1000;

enum myBool { EVEN = 0, ODD = 1 };
typedef enum myBool Even_ODD;

// even odd counts
volatile static Even_ODD times2=EVEN;
volatile static Even_ODD times3=EVEN;
volatile static Even_ODD times4=EVEN;

volatile static int tempUp=0;
volatile static int pulseUp=0;
volatile static int systoDone = 0;

/*typedef struct {
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned int* respirationRateRaw;
} MeasureData;
*/
//typedef struct {
  //signed int* EKGRawBuf;
  //signed int* EKGFreq;
//} EKGData;



void cuff(){

  if (isUp) {
    Pressure = Pressure /10 + Pressure;
    if (Pressure > 150) 
      Pressure = 150;
  }
  else { 
    Pressure = Pressure - Pressure / 10;  
    if (Pressure < 50)
      Pressure = 50;
  }
}

void button_debounce(int* cs, int* ps, int* up, unsigned long* ldt, unsigned long* dd, int pin  ) {
   int reading = digitalRead(pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != *ps) {
    // reset the debouncing timer
    *ldt = millis();
  }
  
  if ((millis() - *ldt) > 50) {
    
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != *cs) {
      
      *cs = reading;

      // only toggle the LED if the new button state is HIGH
      if (*cs == 1) {
        *up = !(*up);
      }
    }
  }
  
  digitalWrite(oPin, *cs);
  *ps = reading;
}
// current state, previous state, led state, last debounce time, decounce delay
void debounce(int* cs, int* ps, int* up, unsigned long* ldt, unsigned long* dd, int pin  ) {
   // read the state of the switch into a local variable:
  int reading = digitalRead(pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != *ps) {
    // reset the debouncing timer
    *ldt = millis();
  }

  if ((millis() - *ldt) > *dd) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != *cs) {
      *cs = reading;

      // only toggle the LED if the new button state is HIGH
        *up = !(*up);
      
    }
  }

 
  

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  *ps = reading;  
}

// due to the limit of interupt ports, we are allowed to use one port to simulate both PR and RR
void pr_detect() {
      int  pulseRateRaw_1;
      //Serial.println("121212121212");
      if (pr_count > 1) {
          unsigned long local_timer = micros();
          pulseRateRaw_1 = 1000000 / (local_timer - global_timer);
          
          global_timer = local_timer;
      }  

      if (functionIndex & 0b1000 ) {
        respirationRateRaw = pulseRateRaw_1;
      } 
      if (functionIndex & 0b0010) {
        pulseRateRaw = pulseRateRaw_1;
      }
      pr_count++;

}   



void measure (char which) {

  //MeasureData* data_in = (MeasureData*) data;
  // temperature
  if (which == 1) {
     temperatureRaw = analogRead(tempPin); 
     delay(5);// read the input pin;   
  }
  
  // pulse rate

  if (which == 3) {
       if (!systoDone) {
          //Serial.println(Pressure);
          if (Pressure  > 110 && Pressure <= 150) {
              systolicPressRaw = analogRead(bpPin);
              delay(5);
             systoDone = !systoDone;
           }
        } else {
          //Serial.println(Pressure);
          if (Pressure  >= 50 && Pressure < 80) {
              diastolicPressRaw = analogRead(bpPin);
              delay(5);
              systoDone = !systoDone;
           }
        }       
  }
  
  
  if (which == 4) {
        /*int cur = 0;
        int cur_High = 0;
        int cur_Low = 0;
        int count = 0;
        unsigned long start = micros();
        while ((micros()) - start < (unsigned long)1000) {
          cur = analogRead(rrPin);     // read the input pin
          if (cur > 70 && cur < 80) {
            curHigh = 1;  
          }
          cur = analogRead(rrPin);
          if (10 < cur && cur < 50) {
            curLow = 1;  
          }
          if (curHigh == 1 && curLow == 1) {
            curHigh = 0;
            curLow = 0;
            count = count + 1;
          }
        }
        *(data_in->respirationRateRaw) = count;*/
      /*  
      int cur = 0;
      int prev = 0;
      int count = 0;
      unsigned long start = micros();
      while ((micros()) - start < (unsigned long)1000) {
          //Serial.println("cur");
          cur = digitalRead(rrPin);     // read the input pin
          //Serial.println(cur);
          if (prev == 0 && cur == 1) {
            count = count + 1;
          }
          prev = cur;
        }
    //Serial.println("count");
    **/
     
    //Serial.println(count); 
   }
}
#endif


