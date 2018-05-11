#include "structures.h"
#include "TimerOne.h"


// menu is tapped
#define MENU_TRUE(x,y) ((y < 900) && (y > 500))
// ANNUNCIATION is tapped
#define ANNUN_TRUE(x,y) ((y < 450) && (y > 0))
// return is pressed
#define BACK_TRUE(x,y) ((y < 180) && (y > 0))
// flags set & unset
#define FLAG(x) x=!x
#define TEMP_FLAG(x,y)  ((y < 900) && (y > 720))
#define PULSE_FLAG(x,y) ((y < 720) && (y > 540))
#define PRESS_FLAG(x,y) ((y < 540) && (y > 360))
#define RESP_FLAG(x,y)  ((y < 360) && (y > 180))
#define ALARM_FLAG(x,y) ((y < 360) && (y > 180))

// values
unsigned char temperatureRawBuf[8];
unsigned char bloodPressRawBuf[16];
unsigned char pulseRateRawBuf[8];
unsigned char respirationRateRawBuf[8];

unsigned char tempCorrectedBuf[8];
unsigned char bloodPressCorrectedBuf[16];
unsigned char pulseRateCorrectedBuf[8];
unsigned char respirationRateCorrectedBuf[8];

unsigned short functionSelectValue;
unsigned short measurementSelection = 0b111;

// pointer values
unsigned short* batteryState_ptr = &batteryState;

// task blocks and task queue
/*
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
*/

// screen state
enum state { TOP = 0, MENU = 1, ANNUN = 2, EXPAND = 3 };
typedef enum state SCR_STATE;
SCR_STATE cur = TOP;
SCR_STATE prev = TOP;

void timerInterrupt() {
  timer++;
  if (sysRed) {
    if (alarmAcknowledge) {
          alarmAcknowledge = alarmAcknowledge - 1;
    }
  }
}

void drawRect (int x, int y, int flag, String text) {
  if (flag) {
    tft.fillRect(x, y, 50, 50, GREEN);
  } else {
     tft.fillRect(x, y, 50, 50, RED);
  }
  tft.setCursor(x, y);
  tft.setTextColor(BLACK);
  tft.print(text);
}

void drawTop() {
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 250, 160, LYELLOW);
  tft.setCursor(70,60);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("MENU");
  tft.fillRect(0, 160, 250, 160, LPURPLE);
  tft.setCursor(60,230);
  tft.print("STATS");
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(0, 0);
}

void drawMenu() {
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  
  tft.setCursor(55, 20);
  if (tempFlag) {
    tft.fillRect(0, 0, 250, 60, GREEN);
  } else {
    tft.fillRect(0, 0, 250, 60, RED);
  }
  tft.print("Tempreture");

  tft.setCursor(55, 84);
  if (pulseFlag) {
    tft.fillRect(0, 64, 250, 60, GREEN);
  } else {
    tft.fillRect(0, 64, 250, 60, RED);
  }
  tft.print("Pulse Rate");

  tft.setCursor(40, 148);
  if (pressFlag) {
    tft.fillRect(0, 128, 250, 60, GREEN);
  } else {
    tft.fillRect(0, 128, 250, 60, RED);
  }
  tft.print("Blood Pressure");

  tft.setCursor(20, 212);
  if (respFlag) {
    tft.fillRect(0, 192, 250, 60, GREEN);
  } else {
    tft.fillRect(0, 192, 250, 60, RED);
  }
  tft.print("Respiration Rate");

  tft.setTextSize(4);
  tft.setCursor(70, 270);
  tft.fillRect(0, 256, 250, 60, LPURPLE);
  tft.print("BACK");
  
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(0, 0);
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
  drawTop();
  pinMode(13, OUTPUT);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timerInterrupt, 1000000);
  Serial1.begin(9600);

  // measure tcb
  mData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, respirationRateRawBuf, &measurementSelection};
  meas = {&measure, &mData};
  // TCB* meas_ptr = &meas;
  // compute tcb
  cData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, respirationRateRawBuf, &measurementSelection, tempCorrectedBuf,  bloodPressCorrectedBuf, pulseRateCorrectedBuf, respirationRateCorrectedBuf};
  comp = {&compute, &cData};
  // TCB* comp_ptr = &comp;
  // display tcb
  dData = {tempCorrectedBuf, bloodPressCorrectedBuf, pulseRateCorrectedBuf, respirationRateCorrectedBuf, batteryState_ptr};
  disp = {&displayF, &dData};
  // TCB* disp_ptr = &disp;
  // warning tcb
  wData = {temperatureRawBuf, bloodPressRawBuf, pulseRateRawBuf, respirationRateRawBuf, batteryState_ptr};
  warn = {&warningAlarm, &wData};
  // TCB* warn_ptr = &warn;
  // status tcb
  sData = {batteryState_ptr};
  stat = {&statusF, &sData};
 
  temp_rb = {1, 0, 0, {0,0,0,0,0,0,0,0}};
  pulse_rb = {1, 0, 0, {0,0,0,0,0,0,0,0}};
  resp_rb = {1, 0, 0, {0,0,0,0,0,0,0,0}};
  
  // TCB* stat_ptr = &stat;
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
        Serial.println(3);
        FLAG(pulseFlag);
      } 
      if (PRESS_FLAG(p.x,p.y)) {
        Serial.println(2);
        FLAG(pressFlag);
      }
      if (RESP_FLAG(p.x, p.y)) {
        FLAG(respFlag);
      }
    } else if (cur == ANNUN) {
      if (ALARM_FLAG(p.x,p.y)) {
        if (sysOutOfRange) {
          alarmAcknowledge = 25;
        }
      }
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      }
      Serial.print("Alarm flag ");Serial.println(alarmAcknowledge);
      Serial.print("Systo flag ");Serial.println(sysOutOfRange);
    }

    if (prev == cur && cur == MENU) {
        drawMenu();
    }
    Serial.print(p.x);
    Serial.print(" ");
    Serial.print(p.y);
    Serial.print("\n");
  }
  
  if (cur != prev) {
    if (cur == TOP) {
      Serial.print("TOP\n");
      drawTop();
    }
    else if (cur == MENU) {
      Serial.print("Menu\n");
      drawMenu();
    } else if (cur == ANNUN) {
      start = timer;
    }
  }
  
  if (cur == ANNUN) {
    Serial.print("ANNU\n");
    scheduler();
  }
  prev = cur;
}
