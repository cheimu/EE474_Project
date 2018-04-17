#include "structures.h"

void measure (void* data) {
  MeasureData* data_in = (MeasureData*) data;
  
  switch(times) {
    case EVEN :
      // temperatureRaw
      if (*(data_in->tempretureRaw) < 50 && *(data_in->tempretureRaw) >= 15) {
        *(data_in->tempretureRaw) += 2;
      } else if (*(data_in->tempretureRaw) < 15) {
        *(data_in->tempretureRaw) -= 2;
      }
      // systolicPressRaw or diastolicPressRaw
      if (*(data_in->systolicPressRaw) <= 100) {
          *(data_in->systolicPressRaw)+=3;
      }else if (*(data_in->systolicPressRaw) > 100 && *(data_in->diastolicPressRaw) < 40) {
        *(data_in->systolicPressRaw) = 80;
        *(data_in->diastolicPressRaw) = 80;
      } else if (*(data_in->systolicPressRaw) > 100 && *(data_in->diastolicPressRaw) >= 40) {
        *(data_in->diastolicPressRaw) -= 2;  
      }
      
      // pulseRateRaw
      if (*(data_in->pulseRateRaw) < 40 && *(data_in->pulseRateRaw) >= 15) {
        *(data_in->pulseRateRaw)--;
      } else if (*(data_in->pulseRateRaw) < 15) {
        *(data_in->tempretureRaw)++;
      }
      times = ODD;
      break;
    case ODD  :
      if (*(data_in->tempretureRaw) < 50 && *(data_in->tempretureRaw) >= 15) {
        *(data_in->tempretureRaw)--;
      } else if (*(data_in->tempretureRaw) < 15) {
        *(data_in->tempretureRaw)++;
      }
      
      // systolicPressRaw or diastolicPressRaw
      if (*(data_in->systolicPressRaw) <= 100) {
          *(data_in->systolicPressRaw)--;
      }else if (*(data_in->systolicPressRaw) > 100 && *(data_in->diastolicPressRaw) < 40) {
        *(data_in->systolicPressRaw) = 80;
        *(data_in->diastolicPressRaw) = 80;
      } else if (*(data_in->systolicPressRaw) > 100 && *(data_in->diastolicPressRaw) >= 40) {
        *(data_in->diastolicPressRaw)++; 
      }
      
      // pulseRateRaw
      if (*(data_in->pulseRateRaw) < 40 && *(data_in->pulseRateRaw) >= 15) {
        *(data_in->pulseRateRaw) += 3;
      } else if (*(data_in->pulseRateRaw) < 15) {
        *(data_in->pulseRateRaw) -= 3;
      }
      times = EVEN;
      break;
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

