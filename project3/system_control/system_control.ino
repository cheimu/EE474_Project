#include "structures.h"
// values
unsigned int temperatureRawBuf[8];
unsigned int bloodPressRawBuf[16];
unsigned int pulseRateRawBuf[8];

unsigned char tempCorrectedBuf[8];
unsigned char bloodPressCorrectedBuf[16];
unsigned char pulseRateCorrectedBuf[8];

unsigned short functionSelect;
unsigned short measurementSelection;
unsigned short alarmAcknowledge;

unsigned short batteryState = 200;
unsigned char bpOutOfRange = 0;
unsigned char tempOutOfRange = 0;
unsigned char pulseOutOfRange = 0;
unsigned char diasOutOfRange = 0;
Bool bpHigh = FALSE;
Bool tempHigh = FALSE;
Bool pulseLow = FALSE;
Bool lowPower = FALSE;

// pointer values
unsigned int* temperatureRaw_ptr = &temperatureRaw;
unsigned int* systolicPressRaw_ptr = &systolicPressRaw;
unsigned int* diastolicPressRaw_ptr = &diastolicPressRaw;
unsigned int* pulseRateRaw_ptr = &pulseRateRaw;
unsigned char* tempCorrected_ptr = tempCorrected;
unsigned char* sysPressCorrected_ptr = systolicPressCorrected;
unsigned char* diastolicPressCorrected_ptr = diastolicPressCorrected;
unsigned char* pulseRateCorrected_ptr = pulseRateCorrected;
unsigned short* batteryState_ptr = &batteryState;

// task blocks and task queue
TCB blocks[5];
TCB meas;
TCB comp;
TCB disp;
TCB warn;
TCB stat;
MeasureData mData;
ComputeData cData;
DisplayData dData;
WarningAlarmData wData;
Status sData;

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
  } else if(identifier==0x0101) {
      identifier=0x9341;
      Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if(identifier==0x1111) {
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
  tft.setTextColor(GREEN); tft.setTextSize(1);
  
}

void loop() {
}
