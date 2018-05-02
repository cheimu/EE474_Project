#include "structures.h"
#include "TimerOne.h"
int timer = 0;


// menu is tapped
#define MENU_TRUE(x,y) ((x < 720) && (x > 330) && (y < 800) && (y > 520))
// ANNUNCIATION is tapped
#define ANNUN_TRUE(x,y) ((x < 720) && (x > 330) && (y < 420) && (y > 150))
// return is pressed
#define BACK_TRUE(x,y) ((y < 200) && (y > 0))
// flags set & unset
#define FLAG(x) x=!x
#define TEMP_FLAG(x,y) ((x < 360) && (x > 160) && (y < 612) && (y > 480))
#define PRESS_FLAG(x,y) ((x < 610 ) && (x > 420) && (y < 612) && (y > 480))
#define PULSE_FLAG(x,y) ((x < 870) && (x > 680) && (y < 612) && (y > 480))



#define ALARM_FLAG(x,y) ((x < 800) && (x > 0) && (y < 612) && (y > 480))



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
  tft.fillRect(90, 50, 80, 80, GREEN);
  tft.fillRect(90, 200, 80, 80, GREEN);
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
  insert(warn_ptr);
  insert(disp_ptr);
  insert(stat_ptr);
}

void drawRect (int x, int y, int flag) {
  if (flag) {
    tft.fillRect(x, y, 50, 50, GREEN);
  } else {
     tft.fillRect(x, y, 50, 50, RED);
  }
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
      if (MENU_TRUE(p.x,p.y)) {
        cur = MENU;
      } else if (ANNUN_TRUE(p.x,p.y)) {
        cur = ANNUN;
      } 
    } else if (cur == MENU) {
      // menu level
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      } 
      if (TEMP_FLAG(p.x,p.y)) {
        Serial.println(1);
        FLAG(tempFlag);
        Serial.println(tempFlag);
      } 
      if (PULSE_FLAG(p.x,p.y)) {
        Serial.println(2);
        FLAG(pulseFlag);
      } 
      if (PRESS_FLAG(p.x,p.y)) {
        Serial.println(3);
        FLAG(pressFlag);
      }
    } else if (cur == ANNUN) {
      if (ALARM_FLAG(p.x,p.y)) {
        alarmAcknowledge = 5;
      }
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      }
    }

    if (prev == cur && cur == MENU) {
      if (TEMP_FLAG(p.x,p.y)) {
        drawRect(10, 100, tempFlag);
      } else if (PRESS_FLAG(p.x,p.y)) {
        drawRect(90, 100, pulseFlag);
      } else if (PULSE_FLAG(p.x,p.y)) {
        drawRect(170, 100, pulseFlag);
      }
    }
    
    Serial.print(p.x);
    Serial.print(" ");
    Serial.print(p.y);
    Serial.print("\n");
  }
  
  if (cur != prev) {
    if (cur == TOP) {
      Serial.print("TOP\n");
      tft.fillScreen(BLACK);
      tft.fillRect(90, 50, 80, 80, GREEN);
      tft.fillRect(90, 200, 80, 80, GREEN);
    }
    else if (cur == MENU) {
      Serial.print("Menu\n");
      tft.fillScreen(BLACK);
      if (tempFlag) {
        tft.fillRect(10, 100, 50, 50, GREEN);
      } else {
        tft.fillRect(10, 100, 50, 50, RED);
      }
      
      if (pulseFlag) {
        tft.fillRect(90, 100, 50, 50, GREEN);
      } else {
        tft.fillRect(90, 100, 50, 50, RED);
      }
      if (pressFlag) {
        tft.fillRect(170, 100, 50, 50, GREEN);
      } else {
        tft.fillRect(170, 100, 50, 50, RED);
      } 
      tft.fillRect(0, 250, 800, 100, CYAN);
     
    }
  }
  
  if (cur == ANNUN) {
    scheduler();
  }
  prev = cur;
}
