#include "structures.h"

void measure (void* data) {
  MeasureData* data_in = (MeasureData*) data;
  unsigned int temp = *(data_in->tempretureRaw);
  if (temp > 50) {
    tempUp = 0;
  } else if (temp < 15) {
    tempUp = 1;
  }
  if (tempUp == 1) {
    if (times == EVEN) {
        *(data_in->tempretureRaw) += 2;
    } else {
        *(data_in->tempretureRaw) -= 1;
    }
  } else {
        if (times == EVEN) {
        *(data_in->tempretureRaw) -= 2;
    } else {
        *(data_in->tempretureRaw) += 1;
    }
  }

  unsigned int pulse = *(data_in->pulseRateRaw);
  if (pulse > 40) {
    pulseUp = 0;
  } else if (pulse < 15) {
    pulseUp = 1;
  }

  if (pulseUp) {
    if (times == ODD) {
        *(data_in->pulseRateRaw) += 3;
    } else {
        *(data_in->pulseRateRaw) -= 1;
    }
  } else {
    if (times == ODD) {
        *(data_in->pulseRateRaw) -= 3;
    } else {
        *(data_in->pulseRateRaw) += 1;
    }
  }



    if (systoDone == 0) {
        if (times == EVEN) {
            *(data_in->systolicPressRaw) += 3;
        } else {
            *(data_in->systolicPressRaw) -= 1;
        }
        if (*(data_in->systolicPressRaw) > 100) {
            systoDone = 1;
            *(data_in->systolicPressRaw) = 80;
        }
    } else {
        if (times == EVEN) {
            *(data_in->diastolicPressRaw) -= 2;
        } else {
            *(data_in->diastolicPressRaw) += 1;
        }
        if (*(data_in->diastolicPressRaw) < 40) {
            systoDone = 0;
            *(data_in->diastolicPressRaw) = 80;
        }
    }




  if (times == EVEN) {
    times = ODD;
  } else {
    times = EVEN;
  }

}


void compute (void* data) {
  ComputeData* data_in = (ComputeData*) data;
  *(data_in->tempCorrected) = 5 + 0.75 * *(data_in->tempretureRaw);
  *(data_in->sysPressCorrected) = 9 + 2 * *(data_in->systolicPressRaw);
  *(data_in->diastolicPressCorrected) = 6 + 1.5 * *(data_in->diastolicPressRaw);
  *(data_in->pulseRateCorrected) = 8 + 3 * *(data_in->pulseRateRaw);  
}

void dispalyF (void* data) {
  DisplayData* data_in = (DisplayData*) data;
  tft.println("Temperature: " + data->tempCorrected " C");
  tft.println("Temperature: " + data->sysPressCorrected + " mmHg");
  tft.println("Temperature: " + data->diastolicPressCorrected + " mmHg");
  tft.println("Temperature: " + data->pulseRateCorrected + " BPM");
  tft.println("Temperature: " + data->batteryState + "%");
  
}

void warningAlarm (void* data) {
  WarningAlarmData* data_in = (WarningAlarmData*) data;
  if (*(data_in->tempretureRaw) < 36.1 || *(data_in->tempretureRaw) > 37.8) {
    if (*(data_in->tempretureRaw) > 37.8) {
      tempHigh = TRUE;  
    }
    tft.setTextColor(RED);
  } else {
    tempHigh = FALSE;
    tft.setTextColor(GREEN);
    
  }
  if (*(data_in->systolicPressRaw) > 120) {
    bpHigh = TRUE;
    tft.setTextColor(RED);
  } else {
    bpHigh = FALSE;
    tft.setTextColor(GREEN);
  }
  if (*(data_in->diastolicPressRaw) < 80) {
    tft.setTextColor(RED);
  } else {
    tft.setTextColor(GREEN); 
  }
  if (*(data_in->pulseRateRaw) < 60  || *(data_in->pulseRateRaw) > 100  ) {
     if (*(data_in->pulseRateRaw) < 60) {
      pulseLow = TRUE;  
    }
    tft.setTextColor(RED);
  } else {
    pulseLow = FALSE;
    tft.setTextColor(GREEN);
    
  }
  if (*(data_in->batteryState) < 20  ) {
    tft.setTextColor(RED);
  } else {
    tft.setTextColor(GREEN); 
  } 
}

void statusF (void* data) {
  Status* data_in = (Status*) data;
  *(data_in->batteryState)--;
}

// precondition: there are five blocks in TCB array
void issue(volatile int* count, int period, int which, TCB* blocks) {
  if (*count == 0) {
    (*blocks[which].myTask)(blocks[which].taskDataPr);
  }
  if (*count == period) {
    *count = 0;
  } else {
    *count++;
  }
}

void scheduler(TCB* blocks) {
  issue(&mCount, mP, 0, blocks);
  issue(&cCount, cP, 1, blocks);
  issue(&wCount, wP, 2, blocks);
  issue(&dCount, dP, 3, blocks);
 
  issue(&sCount, sP, 4, blocks);
  delay(1000);
}

