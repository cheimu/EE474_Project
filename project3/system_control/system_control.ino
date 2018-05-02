#include "structures.h"
#include "TimerOne.h"
int timer = 0;


// menu is tapped
#define MENU_TRUE(x,y) ((x < 680) && (x > 320) && (y < 800) && (y > 620))
// ANNUNCIATION is tapped
#define ANNUN_TRUE(x,y) ((x < 610) && (x > 320) && (y < 480) && (y > 270))
// return is pressed
#define BACK_TRUE(x,y) ((y < 200) && (y > 0))
// flags set & unset
#define FLAG(x) !x
#define TEMP_FLAG(x,y) ((x < 800) && (x > 0) && (y < 800) && (y > 500))
#define PRESS_FLAG(x,y) ((x < 800) && (x > 0) && (y < 800) && (y > 500))
#define PULSE_FLAG(x,y) ((x < 800) && (x > 0) && (y < 800) && (y > 500))
#define ALARM_FLAG(x,y) ((x < 800) && (x > 0) && (y < 800) && (y > 500))



// values
unsigned char temperatureRawBuf[8];
unsigned char bloodPressRawBuf[16];
unsigned char pulseRateRawBuf[8];

unsigned char tempCorrectedBuf[8];
unsigned char bloodPressCorrectedBuf[16];
unsigned char pulseRateCorrectedBuf[8];

unsigned short functionSelectValue;
unsigned short measurementSelection = 0b111;




// pointer values
unsigned short* batteryState_ptr = &batteryState;

// task blocks and task queue
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

// screen state
enum state { TOP = 0, MENU = 1, ANNUN = 2, EXPAND = 3 };
typedef enum state SCR_STATE;
SCR_STATE cur = TOP;
SCR_STATE prev = TOP;

void timerInterrupt() {
  timer++;
  if (mCount == mP) {
    mCount = 0;
  } else {
    mCount +=1;
  }
  if (cCount == cP) {
    cCount = 0;
  } else {
    cCount +=1;
  }
  if (dCount == dP) {
    dCount = 0;
  } else {
    dCount +=1;
  }
  if (wCount == wP) {
    wCount = 0;
  } else {
    wCount +=1;
  }
  if (sCount == sP) {
    sCount = 0;
  } else {
    sCount +=1;
  }
}



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
  tft.fillRect(90, 210, 80, 80, GREEN);
  tft.fillRect(90, 80, 80, 80, GREEN);
  tft.setCursor(0, 0);
  tft.setTextColor(GREEN); tft.setTextSize(1);
  pinMode(13, OUTPUT);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timerInterrupt, 1000000);
  Serial1.begin(9600);


  // measure tcb
  mData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, &measurementSelection};
  meas = {&measure, &mData};
  TCB* meas_ptr = &meas;
  // compute tcb
  cData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, &measurementSelection, tempCorrectedBuf,  bloodPressCorrectedBuf, pulseRateCorrectedBuf};
  comp = {&compute, &cData};
  TCB* comp_ptr = &comp;
  // display tcb
  dData = {tempCorrectedBuf, bloodPressCorrectedBuf, pulseRateCorrectedBuf, batteryState_ptr};
  disp = {&displayF, &dData};
  TCB* disp_ptr = &disp;
  // warning tcb
  wData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, batteryState_ptr};
  warn = {&warningAlarm, &wData};
  TCB* warn_ptr = &warn;
  // status tcb
  sData = {batteryState_ptr};
  stat = {&statusF, &sData};
  TCB* stat_ptr = &stat;
  insert(meas_ptr);
  insert(comp_ptr);
  insert(disp_ptr);
  insert(warn_ptr);
  insert(stat_ptr);
}

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  // UI & mode control
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE){    
    if (cur == TOP) {
      // top level     
      if (MENU_TRUE(p.x,p.y)) 
        cur = MENU;
      else if (ANNUN_TRUE(p.x,p.y))
        cur = ANNUN;
      prev = TOP;
    } else if (cur == MENU) {
      // menu level
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      } else if (TEMP_FLAG(p.x,p.y)) {
        FLAG(tempFlag);
      } else if (PULSE_FLAG(p.x,p.y)) {
        FLAG(pulseFlag);
      } else if (PRESS_FLAG(p.x,p.y)) {
        FLAG(pressFlag);
      }
      prev = MENU;
    } else if (cur == ANNUN) {
      if (ALARM_FLAG(p.x,p.y))
        alarmAcknowledge = 5;
      if (BACK_TRUE(p.x,p.y))
        cur = TOP;
      prev = ANNUN;
    }
    Serial.print(p.x);
    Serial.print(" ");
    Serial.print(p.y);
    Serial.print("\n");
  }
  
  if (cur != prev) {
    if (cur == TOP) {
      Serial.print("TOP\n");
      tft.fillRect(90, 210, 80, 80, GREEN);
      tft.fillRect(90, 80, 80, 80, GREEN);
    }
    else if (cur == MENU) {
      Serial.print("Menu\n");
      tft.fillRect(0, 0, 800, 40, CYAN);
    }
  }
  
  if (cur == ANNUN) {

          Serial.print("ANNUN\n");

      scheduler();
            tft.fillRect(0, 0, 800, 40, CYAN);

  }
}
