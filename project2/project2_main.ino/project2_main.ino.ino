#include "structures.h"
// value initialization
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 75;
unsigned char* tempCorrected = NULL;
unsigned char* systolicPressCorrected = NULL;
unsigned char* diastolicPressCorrected = NULL;
unsigned char* pulseRateCorrected = NULL;
unsigned short batteryState = 200;
// FIX ME
unsigned char bpOutOfRange = 0;
unsigned char tempOutOfRange = 0;
unsigned char pulseOutOfRange = 0;
Bool bpHigh = FALSE;
Bool tempHigh = FALSE;
Bool pulseLow = FALSE;

// pointer initialization
unsigned int* tempretureRaw_ptr = &tempretureRaw;
unsigned int* systolicPressRaw_ptr = &systolicPressRaw;
unsigned int* diastolicPressRaw_ptr = &diastolicPressRaw;
unsigned int* pulseRateRaw_ptr = &pulseRateRaw;
unsigned char** tempCorrected_ptr = &tempCorrected;
unsigned char** sysPressCorrected_ptr = &sysPressCorrected;
unsigned char** diastolicPressCorrected_ptr = &diastolicPressCorrected;
unsigned char** pulseRateCorrected_ptr = &pulseRateCorrected;
unsigned short* batteryState_ptr = batteryState;

TCB Blocks[5];

void setup() {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));


#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

   uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if(identifier==0x1111)
  {     
      identifier=0x9328;
       Serial.println(F("Found 0x9328 LCD driver"));
  }
  else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9328;
  
  }
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(GREEN); tft.setTextSize(2);

  // measure tcb
  void (*measure_ptr)(void*) = &measure;
  MeasureData mData = {tempretureRaw_ptr, systolicPressRaw_ptr, diastolicPressRaw_ptr, pulseRateRaw_ptr};
  TCB meas = {measure_ptr, &mData};

  // compute tcb
  void (*compute_ptr)(void*) = &compute;
  ComputeData cData = {tempretureRaw_ptr, systolicPressRaw_ptr, diastolicPressRaw_ptr, pulseRateRaw_ptr, 
                       tempCorrected_ptr, sysPressCorrected_ptr, diastolicPressCorrected_ptr, pulseRateCorrected_ptr};
  TCB comp = {compute_ptr, &cData};

  // display tcb
  void (*display_ptr)(void*) = &displayF;
  DisplayData dData = {tempCorrected_ptr, sysPressCorrected_ptr, diastolicPressCorrected_ptr, pulseRateCorrected_ptr, batteryState_ptr};
  TCB disp = {display_ptr, &dData};
  
   // warning tcb
  void (*warningAlarm_ptr)(void*) = &warningAlarm;
  WarningAlarmData wData = {tempretureRaw_ptr, systolicPressRaw_ptr, diastolicPressRaw_ptr, pulseRateRaw_ptr, batteryState_ptr};
  TCB warn = {warningAlarm_ptr, &wData};

  // status tcb
   void (*status_ptr)(void*) = &statusF;
   Status sData = {batteryState_ptr};
   TCB stat = {status_ptr, &sData};
  
   blocks[0] = meas;
   blocks[1] = comp;
   blocks[2] = warn;
   blocks[3] = disp;
   blocks[4] = stat;

   
}


void loop() {
  // put your main code here, to run repeatedly:

  
  scheduler(blocks);
}
