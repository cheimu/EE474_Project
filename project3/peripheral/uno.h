#ifndef UNO
#define UNO

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
} MeasureData;

void measure (MeasureData* data_in, char which) {

  if (which == 2) {
    unsigned int temp = *(data_in->temperatureRaw);
    if (temp > 50) {
      tempUp = 0;
    } else if (temp < 15) {
      tempUp = 1;
    }
    if (tempUp == 1) {
      if (times2 == EVEN) {
          *(data_in->temperatureRaw) += 2;
      } else {
          *(data_in->temperatureRaw) -= 1;
      }
    } else {
          if (times2 == EVEN) {
          *(data_in->temperatureRaw) -= 2;
      } else {
          *(data_in->temperatureRaw) += 1;
      }
    }
     if (times2 == EVEN) {
      times2 = ODD;
    } else {
      times2 = EVEN;
    }
  }

  if (which == 4) {
    /*
    unsigned int pulse = *(data_in->pulseRateRaw);
    if (pulse > 40) {
      pulseUp = 0;
    } else if (pulse < 15) {
      pulseUp = 1;
    }
  
    if (pulseUp) {
      if (times4 == ODD) {
          *(data_in->pulseRateRaw) += 3;
      } else {
          *(data_in->pulseRateRaw) -= 1;
      }
    } else {
      if (times4 == ODD) {
          *(data_in->pulseRateRaw) -= 3;
      } else {
          *(data_in->pulseRateRaw) += 1;
      }
    }
    if (times4 == EVEN) {
      times4 = ODD;
    } else {
      times4 = EVEN;
    }
    */

    
  }

  if (which == 3) {
    if (systoDone == 0) {
        if (times3 == EVEN) {
            *(data_in->systolicPressRaw) += 3;
        } else {
            *(data_in->systolicPressRaw) -= 1;
        }
        if (*(data_in->systolicPressRaw) > 100) {
            systoDone = 1;
            *(data_in->systolicPressRaw) = 80;
        }
    } else {
        if (times3 == EVEN) {
            *(data_in->diastolicPressRaw) -= 2;
        } else {
            *(data_in->diastolicPressRaw) += 1;
        }
        if (*(data_in->diastolicPressRaw) < 40) {
            systoDone = 0;
            *(data_in->diastolicPressRaw) = 80;
        }
    }
    if (times3 == EVEN) {
      times3 = ODD;
    } else {
      times3 = EVEN;
    }
  }

}
#endif


