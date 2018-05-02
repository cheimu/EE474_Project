#ifndef STRUCT
#define STRUCT

#include "Elegoo_GFX.h"
#include "Elegoo_TFTLCD.h"
#include "TouchScreen.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// pin assignments
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
// colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// event counters and period
volatile int mCount=0;
volatile int cCount=0;
volatile int dCount=0;
volatile int wCount=0;
volatile int sCount=0;

#define mP 4
#define cP 4
#define dP 4
#define wP 0
#define sP 4

#define MINPRESSURE 10
#define MAXPRESSURE 1000

enum myBool2 {FALSE = 0, TRUE = 1};
typedef enum myBool2 Bool;
unsigned short alarmAcknowledge=0;

unsigned short batteryState = 200;
unsigned char tempOutOfRange = 0;
unsigned char pulseOutOfRange = 0;
unsigned char sysOutOfRange = 0;
unsigned char diasOutOfRange = 0;
Bool tempHigh = FALSE;
Bool pulseLow = FALSE;
Bool lowPower = FALSE;
Bool sysRed = FALSE;


typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
	unsigned short* measurementSelection;
} MeasureData;

typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
	unsigned short* measurementSelection;
	unsigned char* tempCorrectedBuf;
	unsigned char* bloodPressCorrectedBuf;
	unsigned char* pulseRateCorrectedBuf;
} ComputeData;

typedef struct {
	unsigned char* tempCorrectedBuf;
	unsigned char* bloodPressCorrectedBuf;
	unsigned char* pulseRateRawBuf;
	unsigned short* batteryState;
} DisplayData;

typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
	unsigned short* batteryState;
} WarningAlarmData;

typedef struct {
	unsigned short* batteryState;
} Status;

typedef struct {
	unsigned short* measurementSelection;
	unsigned int* alarmAcknowledge;
} TFTKeypadData;

typedef struct {
	unsigned short* measurementSelection;
	unsigned char* tempCorrectedBuf;
	unsigned char* bloodPressCorrectedBuf;
	unsigned char* pulseRateRawBuf;
} CommunicationsData;

struct TCB {
	void (*mytask)(void*);
	void* taskDataPr;
	struct TCB* next;
	struct TCB* prev;
};

typedef struct TCB TCB;

TCB* head = NULL;
TCB* tail = NULL;
void insert(TCB* node);
void deleteNode(TCB* node);

// measure flag
int tempFlag = 1;
int pulseFlag = 1;
int pressFlag = 1;

int functionSelect() {
  int s = 0;
  if (tempFlag)
    s += 0b1;
  if (pulseFlag)
    s += 0b10;
  if (pressFlag)
    s += 0b100;
  return s;
}

void measure(void* data) {
  MeasureData* data_in = (MeasureData*) data;
  *(data_in->measurementSelection) = (char)functionSelect();
  Serial1.write(9);
  Serial1.write(*(data_in->measurementSelection));
  Serial1.write(0);
  
  while (Serial1.available() < 6);
  char inbyte = Serial1.read();
  char temp_rx = Serial1.read();
  char systo_rx = Serial1.read();
  char diasto_rx = Serial1.read();
  char pr_rx = Serial1.read();
  char endbyte = Serial.read();
  
  Serial.print((int)temp_rx);
  Serial.println();
  Serial.print((int)pr_rx);
 
  
  if (*(data_in->measurementSelection) & 0b001) {
     *(data_in->temperatureRawBuf) = temp_rx; 
     
      }
  if (*(data_in->measurementSelection) & 0b010) {
      
     *(data_in->bloodPressRawBuf) = systo_rx;
     *(data_in->bloodPressRawBuf + 8) = diasto_rx;

    }
  if (*(data_in->measurementSelection) & 0b100) {
     *(data_in->pulseRateRawBuf) = pr_rx; 
  }
}

void intToChar(unsigned char* result , int num) {
  result[0] = (unsigned char) (num / 100 + 48);
  result[1] = (unsigned char) (num % 100 / 10 + 48);
  result[2] = (unsigned char) (num % 10 + 48);
}


void compute(void* data) {
  ComputeData* data_in = (ComputeData*) data;
  int tempFixed = (int)*(data_in->temperatureRawBuf);
  tempFixed = 5 + 0.75 * tempFixed;
  int systoFixed = (int) *(data_in->bloodPressRawBuf);
  systoFixed = 9 + 2 * systoFixed;
  int diasFixed = (int) *(data_in->bloodPressRawBuf + 8);
  diasFixed = 6 + 1.5 * diasFixed;
  int pulseFixed = (int) *(data_in->pulseRateRawBuf);
  pulseFixed = 8 + 3 * pulseFixed;
  intToChar(data_in->tempCorrectedBuf, tempFixed);
  intToChar(data_in->bloodPressCorrectedBuf, systoFixed);
  intToChar(data_in->bloodPressCorrectedBuf + 8, diasFixed);
  intToChar(data_in->pulseRateCorrectedBuf, pulseFixed);
}

void displayF (void* data) {
  // calculate whethether systolic or diastolic pressure is out of range
  tft.fillScreen(BLACK);
  tft.setCursor(0,0);
  tft.print("   E-Doc: Your Private Doctor (^ w ^)   ");
  DisplayData* data_in = (DisplayData*) data;
  tft.print("|--------------------------------------|");
  tft.print("|                                      |");
  if (tempFlag) {
    // print temperature
    if (tempOutOfRange == 1) {
      tft.setTextColor(YELLOW);
    } else {
      tft.setTextColor(GREEN);
    }
    tft.print("| Temperature: ");
    tft.print(data_in->tempCorrectedBuf[1]);
    tft.print(data_in->tempCorrectedBuf[2]);
    tft.print(" C                    |");
    tft.setTextColor(GREEN);
    tft.print("|                                      |");
  }
  if (pulseFlag) {
    // print systolic pressure
    if (sysOutOfRange == 1 && alarmAcknowledge != 0) {
      tft.setTextColor(YELLOW);
    } else {
      if (alarmAcknowledge == 0) {
        tft.setTextColor(RED);
      } else {
        tft.setTextColor(GREEN);
      }
    }
    tft.print("| Systolic Pressure: ");
    tft.print(data_in->bloodPressCorrectedBuf[0]);
    tft.print(data_in->bloodPressCorrectedBuf[1]);
    tft.print(data_in->bloodPressCorrectedBuf[2]);
    tft.print(" mmHg          |");
  
    tft.setTextColor(GREEN);
    tft.print("|                                      |");
  
    // print diastolic pressure
    if (diasOutOfRange == 1) {
      tft.setTextColor(YELLOW);
    } else {
      tft.setTextColor(GREEN);
    }
    tft.print("| Diastolic Pressure: ");
    tft.print(data_in->bloodPressCorrectedBuf[8]);
    tft.print(data_in->bloodPressCorrectedBuf[9]);
    tft.print(data_in->bloodPressCorrectedBuf[10]);
    tft.print(" mmHg         |");
    tft.setTextColor(GREEN);
    tft.print("|                                      |");
  }

  if (pressFlag) {
    // print pulse rate
    if (pulseOutOfRange == 1) {
      tft.setTextColor(YELLOW);
    } else {
      tft.setTextColor(GREEN);
    }
    tft.print("| Pulse Rate: ");
    tft.print((char)data_in->pulseRateRawBuf[0]);
    tft.print((char)data_in->pulseRateRawBuf[1]);
    tft.print((char)data_in->pulseRateRawBuf[2]);
    tft.print(" BPM                  |");
    tft.setTextColor(GREEN);
    tft.print("|                                      |");
  }
  // print battery status
  if (lowPower == TRUE) {
    tft.setTextColor(YELLOW);
  } else {
    tft.setTextColor(GREEN);
  }
  
  unsigned char battery[3];
  intToChar(battery, (int)*(data_in->batteryState));
  if ((int)*(data_in->batteryState) > 0) {
    tft.print("| Battery: ");
    tft.print((char)battery[0]);
    tft.print((char)battery[1]);
    tft.print((char)battery[2]);
  } else {
    tft.setTextColor(RED);
    tft.print("| Battery: ");
    tft.print("000");
  }
  tft.print("%                        |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");
  tft.print("|--------------------------------------|");
  tft.print("     We hope you are in good health!    ");
}

void warningAlarm (void* data) {
  WarningAlarmData* data_in = (WarningAlarmData*) data;
  int tempFixed = (int)*(data_in->temperatureRawBuf);
  tempFixed = 5 + 0.75 * tempFixed;
  int systoFixed = (int) *(data_in->bloodPressRawBuf);
  systoFixed = 9 + 2 * systoFixed;
  int diasFixed = (int) *(data_in->bloodPressRawBuf + 8);
  diasFixed = 6 + 1.5 * diasFixed;
  int pulseFixed = (int) *(data_in->pulseRateRawBuf);
  pulseFixed = 8 + 3 * pulseFixed;

  // Temperature
  if (tempFixed < 36.1 || tempFixed > 37.8) {
    tempOutOfRange = 1;
    if ((int) *(data_in->temperatureRawBuf) > 37.8) {
      tempHigh = TRUE;
    }
  } else {
    tempOutOfRange = 0;
    tempHigh = FALSE;
  }

  // BP
  if (systoFixed > 130 || systoFixed < 120 || diasFixed > 80 || diasFixed <70) {
    if (diasFixed < 70 || diasFixed > 80) {
      diasOutOfRange = 1;
    } else {
      diasOutOfRange = 0;
    }
    if (systoFixed > 130 || systoFixed < 120) {
      sysOutOfRange = TRUE;
      if (alarmAcknowledge) {
        alarmAcknowledge = alarmAcknowledge - 1;
      }
      if (systoFixed > 156 || systoFixed < 96) {
        sysRed = TRUE;
      } 
    } else {
      alarmAcknowledge = 0;
      sysOutOfRange = FALSE;
    }
  } else {
    sysOutOfRange = 0;
    diasOutOfRange = 0;
  }

  // Palse Rate
  if (pulseFixed < 60  || pulseFixed > 100) {
    pulseOutOfRange = 1;
     if (pulseFixed < 60) {
      pulseLow = TRUE;
    }
  } else {
    pulseOutOfRange = 0;
    pulseLow = FALSE;
  }

  // Battery
  if (*(data_in->batteryState) < 20  ) {
    lowPower = TRUE;
  } else {
    lowPower = FALSE;
  }
}

void statusF (void* data) {
  Status* data_in = (Status*) data;
  if (data_in->batteryState > 0) {
    *(data_in->batteryState) -= 1;
  }
}

void issue(volatile int* count, TCB* blocks) {
  if (*count == 0) {
    (*blocks->mytask)(blocks->taskDataPr);
  }
}

void scheduler() {
  TCB* cur = head;
  issue(&mCount, cur);
  cur = cur->next;
  issue(&cCount, cur);
  cur = cur->next;
  issue(&wCount, cur);
  cur = cur->next;
  issue(&dCount, cur);
  cur = cur->next;
  issue(&sCount, cur);
}
#endif
