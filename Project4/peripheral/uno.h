#ifndef UNO
#define UNO
char inbyte;
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 75;
unsigned int respirationRateRaw =25;
int tempPin = A3;
int bpPin = A4;
int prPin = 7;
int rrPin = 8;
int val = 0;

enum myBool { EVEN = 0, ODD = 1 };
typedef enum myBool Even_ODD;

// even odd counts
volatile static Even_ODD times2=EVEN;
volatile static Even_ODD times3=EVEN;
volatile static Even_ODD times4=EVEN;

volatile static int tempUp=0;
volatile static int pulseUp=0;
volatile static int systoDone = 0;

typedef struct {
  unsigned int* temperatureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned int* respirationRateRaw;
} MeasureData;

void measure (MeasureData* data_in, char which) {

  MeasureData* data_in = (MeasureData*) data;
  // temperature
  if (which == 1) {
   
         
        *(data_in->temperatureRaw) =  analogRead(tempPin);     // read the input pin;   
  }

  // blood pressure
 // if (which == 2) {
      
       
          
    //    pulseRateRaw = analogRead(bpPin);     // read the input pin
          

    
  //}

  if (which == 3) {
     unsigned long start = micros();
        int cur = 0;
        int prev = 0;
        int count = 0;
        while ((micros()) - start < (unsigned long)1000) {
          cur = digitalRead(prPin);     // read the input pin
          if (prev == 0 && cur == 1) {
            count = count + 1;
          }
          prev = cur;
        }
        *(data_in->pulseRateRaw) = count;   
  }

  if (which == 4) {
     unsigned long start = micros();
        int cur = 0;
        int prev = 0;
        int count = 0;
        while ((micros()) - start < (unsigned long)1000) {
          cur = digitalRead(rrPin);     // read the input pin
          if (prev == 0 && cur == 1) {
            count = count + 1;
          }
          prev = cur;
        }
        *(data_in->respirationRateRaw) = count;   
  }

}
#endif


