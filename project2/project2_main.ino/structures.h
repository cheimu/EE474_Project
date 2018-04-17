#ifndef STRUCT
#define STRUCT
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>

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

enum myBool { EVEN = 0, ODD = 1 };
typedef enum myBool Bool;

// even odd counts
volatile static Bool times=EVEN;



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

typedef struct {
  unsigned int* tempretureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
} MeasureData;

typedef struct {
  unsigned int* tempretureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned char** tempCorrected;
  unsigned char** sysPressCorrected;
  unsigned char** diastolicPressCorrected;
  unsigned char** pulseRateCorrected;
} ComputeData;

typedef struct {
  unsigned char** tempCorrected;
  unsigned char** sysPressCorrected;
  unsigned char** diastolicPressCorrected;
  unsigned char** pulseRateCorrected;
  unsigned short* batteryState;
} DisplayData;

typedef struct {
  unsigned int* tempretureRaw;
  unsigned int* systolicPressRaw;
  unsigned int* diastolicPressRaw;
  unsigned int* pulseRateRaw;
  unsigned short* batteryState;
} WarningAlarmData;

typedef struct {
  unsigned short* batteryState;
} Status;

typedef struct {
} SchedulerData;

typedef struct {
  void (*myTask)(void*);
  void* taskDataPr;
} TCB;

void measure(void* data);
void compute(void* data);
void displayF(void* data);
void warning_alarm(void* data);
void statusF(void* data);
void scheduler(TCB* blocks);
#endif
