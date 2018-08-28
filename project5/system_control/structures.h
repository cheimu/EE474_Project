#ifndef STRUCT
#define STRUCT

#include "optfft.h"
// menu is tapped
#define MENU_TRUE(x,y) ((y < 900) && (y > 600))
// ANNUNCIATION is tapped
#define ANNUN_TRUE(x,y) ((y < 600) && (y > 300))
// Display is tapped
#define DIS_TRUE(x,y) ((y < 300) && (y > 0))

// return is pressed
#define BACK_TRUE(x,y) ((y < 150) && (y > 0))
// flags set & unset
#define FLAG(x) x=!x
#define TEMP_FLAG(x,y)  ((y < 900) && (y > 750))
#define PULSE_FLAG(x,y) ((y < 750) && (y > 600))
#define PRESS_FLAG(x,y) ((y < 600) && (y > 450))
#define RESP_FLAG(x,y)  ((y < 450) && (y > 300))
#define EKG_FLAG(x,y) ((y < 300) && (y > 150))

#define ALARM_FLAG(x,y) ((y < 300) && (y > 150))


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
#define ORANGE  0xFDAA
#define LYELLOW 0xFFD7
#define LPURPLE 0xAC7F

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// event counters and period
volatile int mCount=0;
volatile int cCount=0;
volatile int dCount=0;
volatile int wCount=0;
volatile int sCount=0;

volatile int mPrev=4;
volatile int cPrev=0;
volatile int dPrev=4;
volatile int wPrev=4;
volatile int sPrev=4;



int ekg_buffer[16];


void resetZeros(signed int zero[]) {
   for (int i = 0; i < 256; i++) {
      zero[i] = 0;  
  }  
}


#define MINPRESSURE 10
#define MAXPRESSURE 1000

enum myBool2 {FALSE = 0, TRUE = 1};
typedef enum myBool2 Bool;
unsigned short alarmAcknowledge=0;

// screen state
enum state { TOP = 0, MENU = 1, ANNUN = 2, DIS = 3, NONE = 4, NODIS};
typedef enum state SCR_STATE;
SCR_STATE cur = NONE;
SCR_STATE prev = NONE;
SCR_STATE saved = NONE;

unsigned short batteryState = 200;
unsigned char tempOutOfRange = 0;
unsigned char pulseOutOfRange = 0;
unsigned char sysOutOfRange = 0;
unsigned char diasOutOfRange = 0;
unsigned char rrOutOfRange = 0;
unsigned char EKGOutOfRange = 0;

Bool tempHigh = FALSE;
Bool tempLow = FALSE;
Bool rrLow = FALSE;
Bool rrHigh = FALSE;
Bool pulseLow = FALSE;
Bool pulseHigh = FALSE;
Bool lowPower = FALSE;
Bool sysRed = FALSE;
Bool diasRed = FALSE;
Bool EKGLow = FALSE;
Bool EKGHigh = FALSE;

// pulse ring buffer
struct ringBuffer {
  int lead;
  int bufHead;
  int bufTail;
  char inpulseBuffer[8];
};


typedef struct ringBuffer ringBuffer;
ringBuffer pulse_rb;
ringBuffer temp_rb;
ringBuffer resp_rb;
//ringBuffer* pulse_rb_ptr = &pulse_rb;
//ringBuffer* temp_rb_ptr = &temp_rb;
//ringBuffer* resp_rb_ptr = &resp_rb;

void put_data(char val, int size_i, ringBuffer* buf);
char get_data(int size_i, ringBuffer* buf);

char pulsePrev = 0;
char tempPrev = 0;
char respPrev = 0;

// timers and scheduling flags
unsigned long timer = 0;
unsigned long timer_prev = 19;
unsigned long interval = 0;
unsigned long start = 0;
Bool measureFlag = FALSE;
Bool computeFlag = FALSE;
Bool disFlag = FALSE;
Bool statusFlag = FALSE;
Bool warnFlag = FALSE;
Bool EKGCap = FALSE;
Bool EKGPro = FALSE;
Bool EKGCapFlag = FALSE;
Bool EKGProFlag = FALSE; // for measure use

typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
  unsigned char* respirationRateRawBuf;
  unsigned char* EKGFreqBuf;
	unsigned short* measurementSelection;
} MeasureData;

typedef struct {
  unsigned char* EKGRawBuf;
  unsigned char* EKGFreqBuf;
} EKGData;

typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
  unsigned char* respirationRateRawBuf;
	unsigned short* measurementSelection;
	unsigned char* tempCorrectedBuf;
	unsigned char* bloodPressCorrectedBuf;
	unsigned char* pulseRateCorrectedBuf;
  unsigned char* respirationRateCorrectedBuf;
  unsigned char* EKGFreqBuf;
  unsigned char* EKGCorrectedFreqBuf;
} ComputeData;

typedef struct {
	unsigned char* tempCorrectedBuf;
	unsigned char* bloodPressCorrectedBuf;
	unsigned char* pulseRateCorrectedBuf;
  unsigned char* respirationRateCorrectedBuf;
  unsigned char* EKGCorrectedFreqBuf;
	unsigned short* batteryState;
} DisplayData;

typedef struct {
	unsigned char* temperatureRawBuf;
	unsigned char* bloodPressRawBuf;
	unsigned char* pulseRateRawBuf;
  unsigned char* respirationRateRawBuf;
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
	unsigned char* pulseRateCorrectedBuf;
  unsigned char* respirationRateCorrectedBuf;
} CommunicationsData;

struct TCB {
	void (*mytask)(void*);
	void* taskDataPr;
	struct TCB* next;
	struct TCB* prev;
};

typedef struct TCB TCB;

unsigned char temp_r= 78;
unsigned char sys_r = 78;
unsigned char dias_r = 67;
unsigned char pulse_r = 67;
unsigned char resp_r = 45;
int ekg_r = 56;

int tempWr=0;
int sysWr=0;
int disWr=0;
int pulsrW=0;
int resWr=0;
int ekgWr=0;

typedef struct {
  unsigned char* tempCorrectedBuf;
  unsigned char* bloodPressCorrectedBuf;
  unsigned char* pulseRateCorrectedBuf;
  unsigned char* respirationRateCorrectedBuf;
  unsigned char* EKGCorrectedFreqBuf;
  unsigned char* temp_d;
  unsigned char* sys_d;
  unsigned char* dias_d;
  unsigned char* pulse_d;
  unsigned char* resp_d;
  int* ekg_d;
  int* temp_f;
  int* sys_f;
  int* dis_f;
  int* pul_f;
  int* res_f;
  int* ekg_f;
  int* tempW;
  int* sysW;
  int* disW;
  int* pulsW;
  int* resW;
  int* ekgW;
} RemoteComData;

int warnUp = 0;
int dataUp = 0;

void remoteCom(void* data);

typedef struct {
  unsigned char* temp_d;
  unsigned char* sys_d;
  unsigned char* dias_d;
  unsigned char* pulse_d;
  unsigned char* resp_d;
  int* ekg_d;
  int* tempW;
  int* sysW;
  int* disW;
  int* pulsW;
  int* resW;
  int* ekgW;
} RemoteDisData;

void remoteDis(void* data_in);
TCB* head = NULL;
TCB* tail = NULL;
void insert(TCB* node);
void deleteNode(TCB* node);


// measure flag
int tempFlag = 1;
int pulseFlag = 1;
int pressFlag = 1;
int respFlag = 1;
int ekgFlag = 0;

int EKGPin = A15;

// task blocks and task queue
TCB meas;
TCB EKG_process;
TCB EKG_capture;
TCB comp;
TCB disp;
TCB warn;
TCB stat;
TCB key;
TCB fla;
TCB co;
TCB reDispl;
TCB reCOM;

signed int EKGRawBuf[256];
signed int zeros[256];

MeasureData mData;
ComputeData cData;
DisplayData dData;
WarningAlarmData wData;
Status sData;
RemoteDisData redisD;
RemoteComData recomD;

int command_enable = 0;
int error_message = 0;
int dis_enable = 1;
void command (void* data);
int is_first =0;

void intToChar(unsigned char* result , int num) {
  result[0] = (unsigned char) (num / 100 + 48);
  result[1] = (unsigned char) (num % 100 / 10 + 48);
  result[2] = (unsigned char) (num % 10 + 48);
}

void intToChar4(unsigned char* result , int num) {
  result[0] = (unsigned char) (num / 1000 + 48);
  result[1] = (unsigned char) (num /100 % 10  + 48);
  result[2] = (unsigned char) (num /10 % 10 + 48);
  result[3] = (unsigned char) (num % 10 + 48);
}
int functionSelect() {
  int s = 0;
  if (tempFlag)
    s += 0b1;
  if (pulseFlag)
    s += 0b10;
  if (pressFlag)
    s += 0b100;
  if (respFlag)
    s += 0b1000;
  return s;
}

void drawRect (int x, int y, int flag) {
  if (flag) {
    tft.fillRect(x, y, 250, 50, GREEN);
  } else {
     tft.fillRect(x, y, 250, 50, RED);
  }
}

void drawTop() {
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 250, 160, LYELLOW);
  tft.setCursor(80,50);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("MENU");

  
  tft.fillRect(0, 110, 250, 160, LPURPLE);
  tft.setCursor(80,150);
  tft.print("ANNU");

  tft.fillRect(0, 220, 250, 160, LYELLOW);
  tft.setCursor(55,260);
  tft.print("MEASURE");

  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(0, 0);
}

void drawMenu() {
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);

  tft.setCursor(55, 15);
  drawRect(0, 0, tempFlag);
  tft.print("Tempreture");

  tft.setCursor(55, 69);
  drawRect(0, 54, pulseFlag);
  tft.print("Pulse Rate");

  tft.setCursor(40, 123);
  drawRect(0, 108, pressFlag);
  tft.print("Blood Pressure");

  tft.setCursor(20, 177);
  drawRect(0, 162, respFlag);
  tft.print("Respiration Rate");

  tft.setCursor(95, 231);
  drawRect(0, 216, ekgFlag);
  tft.print("EKG");

  tft.setTextSize(4);
  tft.setCursor(70, 280);
  tft.fillRect(0, 270, 250, 50, LPURPLE);
  tft.print("BACK");

  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(0, 0);
}

void drawDis(void* data) {
  DisplayData* data_in = (DisplayData*) data;
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);
  tft.setCursor(0,0);
  tft.print("   E-Doc: Your Private Doctor (^ w ^)   ");
  tft.print("|--------------------------------------|");
  tft.print("|                                      |");

  tft.print("| Temperature: ");
  tft.print((char)data_in->tempCorrectedBuf[1]);
  tft.print((char)data_in->tempCorrectedBuf[2]);
  tft.print(" C                    |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");
  
  tft.print("| Systolic Pressure: ");
  tft.print((char)data_in->bloodPressCorrectedBuf[0]);
  tft.print((char)data_in->bloodPressCorrectedBuf[1]);
  tft.print((char)data_in->bloodPressCorrectedBuf[2]);
  tft.print(" mmHg          |");

  tft.print("|                                      |");

  tft.print("| Diastolic Pressure: ");
  tft.print((char)data_in->bloodPressCorrectedBuf[8]);
  tft.print((char)data_in->bloodPressCorrectedBuf[9]);
  tft.print((char)data_in->bloodPressCorrectedBuf[10]);
  tft.print(" mmHg         |");
  tft.print("|                                      |");

  tft.print("| Pulse Rate: ");
  tft.print((char)data_in->pulseRateCorrectedBuf[0]);
  tft.print((char)data_in->pulseRateCorrectedBuf[1]);
  tft.print((char)data_in->pulseRateCorrectedBuf[2]);
  tft.print(" BPM                  |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");

  tft.print("| Respiration Rate: ");
  tft.print((char)data_in->respirationRateCorrectedBuf[0]);
  tft.print((char)data_in->respirationRateCorrectedBuf[1]);
  tft.print((char)data_in->respirationRateCorrectedBuf[2]);
  tft.print(" BPM            |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");

  tft.print("| EKG: ");
  tft.print((char)data_in->EKGCorrectedFreqBuf[0]);
  tft.print((char)data_in->EKGCorrectedFreqBuf[1]);
  tft.print((char)data_in->EKGCorrectedFreqBuf[2]);
  tft.print((char)data_in->EKGCorrectedFreqBuf[3]);
  tft.print(" Hz                         |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");

  unsigned char battery[3];
  intToChar(battery, (int)*(data_in->batteryState));
  if ((int)*(data_in->batteryState) > 0) {
    tft.print("| Battery: ");
    tft.print((char)battery[0]);
    tft.print((char)battery[1]);
    tft.print((char)battery[2]);
  } else {
    tft.print("| Battery: ");
    tft.print("000");
  }
  tft.print("%                        |");
  tft.setTextColor(GREEN);
  tft.print("|                                      |");
  tft.print("|--------------------------------------|");
  tft.print("     We hope you are in good health!    ");

  // Back area
  tft.setTextSize(4);
  tft.setCursor(70, 285);
  tft.fillRect(0, 270, 250, 60, LPURPLE);
  tft.print("BACK");

  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(0, 0);
}

void ekg_capture(void* data) {
  EKGCapFlag = TRUE;
   for (int i = 0; i < 256; i++) {
      long mark = micros();
      double rawReading = (analogRead(EKGPin)- 512) * 21.0 / 341.0 + 0.53;
      EKGRawBuf[i] = (signed int) rawReading;
      while (micros() - mark < (1.0/10000.0)*1000000.0);
   }
   EKGProFlag = TRUE;
}

unsigned int EKGFreq;
int ekg_pointer=0;
void ekg_process(void* data) {
  resetZeros(zeros);
   EKGFreq = (unsigned int)optfft(EKGRawBuf, zeros); 
   ekg_buffer[ekg_pointer] = (unsigned int) EKGFreq * 10000.0 / 256.0; // 9523 from sample period 105 micro second (see delay above)
   if (ekg_pointer == 16) {
    ekg_pointer = 0;
   } else {
    ekg_pointer++;
   }
    EKGProFlag = FALSE;
}

void measure(void* data) {
  computeFlag = TRUE;
  disFlag = TRUE;
  statusFlag = TRUE;
  warnFlag = TRUE;
  MeasureData* data_in = (MeasureData*) data;
  *(data_in->measurementSelection) = (char)functionSelect();
  
  Serial1.write((char)9);
  Serial1.write(*(data_in->measurementSelection));
  Serial1.write(0);


  while (Serial1.available() < 8);  
  
  
  
  char inbyte = Serial1.read();
  char temp_rx = Serial1.read();
  char cuff = Serial1.read();
  char systo_rx = Serial1.read();
  char diasto_rx = Serial1.read();
  char pr_rx = Serial1.read();
  char rr_rx = Serial1.read();
  

  /*
  char inbyte = 9;
  char temp_rx = 100;
  char cuff = 100;
  char systo_rx = 98;
  char diasto_rx = 100;
  char pr_rx = 123;
  char rr_rx = 200;
  char EKG_rx = 1;
  char endbyte = 0;
  */
  
  char endbyte = Serial1.read();
  
    /*
    Serial.println("Received");
    Serial.print("begin in");Serial.print(" ");
    Serial.print((unsigned int)inbyte); Serial.print(" ");
    Serial.print("end in");
    Serial.print((unsigned int)temp_rx);Serial.print(" ");
    Serial.print((unsigned int)cuff);Serial.print(" ");
    Serial.print((unsigned int)systo_rx);Serial.print(" ");
    Serial.print((unsigned int)diasto_rx);Serial.print(" ");
    Serial.print((unsigned int)pr_rx);Serial.print(" ");
    Serial.print((unsigned int)rr_rx);Serial.print(" ");
    Serial.print((unsigned int)endbyte);Serial.print(" ");
    Serial.println(); 
    */

//   char temp_rx = 62;
//   char systo_rx = 10000;
//   char diasto_rx = 90;
//   char pr_rx = 30;
//   char rr_rx = 10;

  if (*(data_in->measurementSelection) & 0b0001) {
     data_in->temperatureRawBuf[0] = temp_rx;
   }

  if (*(data_in->measurementSelection) & 0b0100) {

     *(data_in->bloodPressRawBuf) = systo_rx;
     *(data_in->bloodPressRawBuf + 8) = diasto_rx;

    }
  if (*(data_in->measurementSelection) & 0b0010) {
     *(data_in->pulseRateRawBuf) = pr_rx;
  }
   if (*(data_in->measurementSelection) & 0b1000) {
     *(data_in->respirationRateRawBuf) = rr_rx;
  }
//  Serial.println(F("Measured"));
//  Serial.print((int)data_in->temperatureRawBuf[0]); Serial.print(" ");
//  Serial.print((int)data_in->bloodPressRawBuf[0]);Serial.print(" ");
//  Serial.print((int)data_in->bloodPressRawBuf[8]);Serial.print(" ");
//  Serial.print((int)data_in->pulseRateRawBuf[0]);Serial.print(" ");
//  Serial.print((int)data_in->respirationRateRawBuf[0]);Serial.print(" ");
//  Serial.println();
}

void keyPad(void* data) {
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
      } else {
        cur = DIS;
      }
    } else if (cur == MENU) {
      tft.setTextColor(BLACK);
      tft.setTextSize(2);
      // menu level
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      }
      if (TEMP_FLAG(p.x,p.y)) {
        FLAG(tempFlag);
        tft.setCursor(55, 15);
        drawRect(0, 0, tempFlag);
        tft.print("Tempreture");
      }
      if (PULSE_FLAG(p.x,p.y)) {
        FLAG(pulseFlag);
        tft.setCursor(55, 69);
        drawRect(0, 54, pulseFlag);
        tft.print("Pulse Rate");
      }
      if (PRESS_FLAG(p.x,p.y)) {
        FLAG(pressFlag);
        tft.setCursor(40, 123);
        drawRect(0, 108, pressFlag);
        tft.print("Blood Pressure");
      }
      if (RESP_FLAG(p.x, p.y)) {
        FLAG(respFlag);
        tft.setCursor(20, 177);
        drawRect(0, 162, respFlag);
        tft.print("Respiration Rate");
      }
     
      if (EKG_FLAG(p.x, p.y)) {
        FLAG(ekgFlag);
        if (EKGCapFlag == TRUE) {
          EKGCapFlag = FALSE;
        } else {
          EKGCapFlag = TRUE;
        }
        tft.setCursor(95, 231);
        drawRect(0, 216, ekgFlag);
        tft.print("EKG");

      }
      
    } else if (cur == ANNUN) {
      if (ALARM_FLAG(p.x,p.y)) {

        if (sysOutOfRange || diasOutOfRange || pulseOutOfRange || tempOutOfRange) {
          alarmAcknowledge = 100;
        }
      }
      if (BACK_TRUE(p.x,p.y)) {
        cur = TOP;
      }

      //Serial.print("Alarm flag ");Serial.println(alarmAcknowledge);
      //Serial.print("Systo flag ");Serial.println(sysOutOfRange);
    } else if (cur == DIS) {
        if (BACK_TRUE(p.x,p.y)) {
          cur = TOP;
        }
    }
  }

  if (cur != prev) {
    if (cur == TOP || cur == NONE) {
      if (dis_enable) {
        Serial.print(F("TOP\n"));
        drawTop();
      }
    }
    else if (cur == MENU) {
      Serial.print(F("Menu\n"));
      drawMenu();
    } else if (cur == ANNUN) {
      Serial.println(F("start reset"));
      if (is_first == 0) {
        tft.fillScreen(BLACK); 
        tft.setCursor(0,0);
        tft.print("calibrating..........");
        is_first = 1;
      }
      //measureFlag = TRUE;
    } else if (cur == DIS){
      measureFlag = TRUE;
    }
  }
}

int temp_f;
int sys_f;
int dis_f;
int pul_f;
int res_f;
int ekg_f;


void compute(void* data) {
  ComputeData* data_in = (ComputeData*) data;
  int tempFixed = (int)*(data_in->temperatureRawBuf);
  tempFixed = 30 + tempFixed / 10;
  int systoFixed = (int) *(data_in->bloodPressRawBuf);
  systoFixed = 92 + systoFixed * 34 / 115;
  int diasFixed = (int) *(data_in->bloodPressRawBuf + 8);
  diasFixed = 50 + diasFixed * 26/115;
  int pulseFixed = (int) *(data_in->pulseRateRawBuf);
  pulseFixed = 116 / 3 + pulseFixed * 37 / 15;
  int respFixed = (int) *(data_in->respirationRateRawBuf);
  unsigned int EKGFixed = EKGFreq;
  EKGFixed = (EKGFixed * 10000.0 / 256.0)* 0.6093 + 10;
    
  
  temp_f = tempFixed;
  sys_f = systoFixed;
  dis_f = diasFixed;
  pul_f = pulseFixed;
  res_f = respFixed;
  ekg_f = EKGFixed;
  // add to buffer
  if (pulseFixed > (1.15 * pulsePrev) || pulseFixed < (0.85 * pulsePrev)) {
    put_data((char)pulseFixed, 9, &pulse_rb);
    pulsePrev = pulseFixed;
  }
  if (tempFixed > (1.15 * tempPrev) || tempFixed < (0.85 * tempPrev)) {
    put_data((char)tempFixed, 9, &temp_rb);
    tempPrev = tempFixed;
  }
  if (respFixed > (1.15 * respPrev) || respFixed < (0.85 * respPrev)) {
    put_data((char)respFixed, 9, &resp_rb);
    respPrev = respFixed;
  }

//  Serial.println(F("Fixed"));
//  Serial.print(tempFixed); Serial.print(F(" "));
//  Serial.print(systoFixed);Serial.print(F(" "));
//  Serial.print(diasFixed);Serial.print(F(" "));
//  Serial.print(pulseFixed);Serial.print(F(" "));
//  Serial.print(respFixed);Serial.print(F(" "));
//  Serial.println();

  intToChar(data_in->tempCorrectedBuf, tempFixed);
  intToChar(data_in->bloodPressCorrectedBuf, systoFixed);
  intToChar(data_in->bloodPressCorrectedBuf + 8, diasFixed);
  intToChar(data_in->pulseRateCorrectedBuf, pulseFixed);
  intToChar(data_in->respirationRateCorrectedBuf, respFixed);
  intToChar4(data_in->EKGCorrectedFreqBuf, EKGFixed);

//  Serial.println(F("Corrected"));
//  Serial.print(data_in->tempCorrectedBuf[0]);Serial.print(data_in->tempCorrectedBuf[1]);Serial.print(data_in->tempCorrectedBuf[2]); Serial.print(" ");
//  Serial.print(data_in->bloodPressCorrectedBuf[0]);Serial.print(data_in->bloodPressCorrectedBuf[1]);Serial.print(data_in->bloodPressCorrectedBuf[2]);Serial.print(" ");
//  Serial.print(data_in->bloodPressCorrectedBuf[0]);Serial.print(data_in->bloodPressCorrectedBuf[1]);Serial.print(data_in->bloodPressCorrectedBuf[2]);Serial.print(" ");
//  Serial.print(data_in->pulseRateCorrectedBuf[0]);Serial.print(data_in->pulseRateCorrectedBuf[1]);Serial.print(data_in->pulseRateCorrectedBuf[2]);Serial.print(" ");
//  Serial.println();
}

void displayF (void* data) {
  if (cur == ANNUN) {
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
        if ((tempHigh || tempLow) && alarmAcknowledge == 0) {
          tft.setTextColor(RED);
          tft.print("| Temperature: ");
          tft.print((char)data_in->tempCorrectedBuf[1]);
          tft.print((char)data_in->tempCorrectedBuf[2]);
          tft.print(" C                    |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |");
        } else {
          tft.setCursor(0, 40);
        }
      } else {
        tft.setTextColor(GREEN);
        tft.print("| Temperature: ");
        tft.print((char)data_in->tempCorrectedBuf[1]);
        tft.print((char)data_in->tempCorrectedBuf[2]);
        tft.print(" C                    |");
        tft.setTextColor(GREEN);
        tft.print("|                                      |");
      }
    }
    
    if (pressFlag) {
      // print systolic pressure
      if (sysOutOfRange == 1 && !sysRed) {
        
        if (tempFlag) {
          tft.setCursor(0, 56);
        } else {
          tft.setCursor(0, 40);
        }
      } else if (sysRed == 1 && alarmAcknowledge == 0) {
          tft.setTextColor(RED);
          
          tft.print("| Systolic Pressure: ");
          tft.print((char)data_in->bloodPressCorrectedBuf[0]);
          tft.print((char)data_in->bloodPressCorrectedBuf[1]);
          tft.print((char)data_in->bloodPressCorrectedBuf[2]);
          tft.print(" mmHg          |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |");
          
      } else if (sysRed == 1 && alarmAcknowledge != 0) {
        
          if (tempFlag) {
            tft.setCursor(0, 56);
          } else {
            tft.setCursor(0, 40);
          } 
      } else{
          tft.setTextColor(GREEN);
          
          tft.print("| Systolic Pressure: ");
          tft.print((char)data_in->bloodPressCorrectedBuf[0]);
          tft.print((char)data_in->bloodPressCorrectedBuf[1]);
          tft.print((char)data_in->bloodPressCorrectedBuf[2]);
          tft.print(" mmHg          |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |"); 
      }
    
      // print diastolic pressure
      if (diasOutOfRange == 1 && !diasRed) {
        
        if (tempFlag) {
          tft.setCursor(0, 72);
        } else {
          tft.setCursor(0, 56);
        }
      } else if (diasRed == TRUE && alarmAcknowledge == 0) {
          tft.setTextColor(RED);
          
          tft.print("| Diastolic Pressure: ");
          tft.print((char)data_in->bloodPressCorrectedBuf[8]);
          tft.print((char)data_in->bloodPressCorrectedBuf[9]);
          tft.print((char)data_in->bloodPressCorrectedBuf[10]);
          tft.print(" mmHg         |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |");
          
      } else if (diasRed == TRUE && alarmAcknowledge != 0) {
        
          if (tempFlag) {
            tft.setCursor(0, 72);
          } else {
            tft.setCursor(0, 56);
          } 
      } else {
        tft.setTextColor(GREEN);
        
        tft.print("| Diastolic Pressure: ");
        tft.print((char)data_in->bloodPressCorrectedBuf[8]);
        tft.print((char)data_in->bloodPressCorrectedBuf[9]);
        tft.print((char)data_in->bloodPressCorrectedBuf[10]);
        tft.print(" mmHg         |");
        tft.setTextColor(GREEN);
        tft.print("|                                      |");
        
      }
    }
  
    if (pulseFlag) {
      // print pulse rate
      if (pulseOutOfRange == 1) {
        if((pulseLow || pulseHigh) && alarmAcknowledge == 0) {
          tft.setTextColor(RED);
          tft.print("| Pulse Rate: ");
          tft.print((char)data_in->pulseRateCorrectedBuf[0]);
          tft.print((char)data_in->pulseRateCorrectedBuf[1]);
          tft.print((char)data_in->pulseRateCorrectedBuf[2]);
          tft.print(" BPM                  |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |");
        } else {
          if (tempFlag && pressFlag) {
            tft.setCursor(0,88);
          } else if (tempFlag) {
            tft.setCursor(0, 56);
          } else if (pressFlag) {
            tft.setCursor(0,72);
          } else {
            tft.setCursor(0, 40);
          } 
        }
      } else {
        tft.setTextColor(GREEN);
        tft.print("| Pulse Rate: ");
        tft.print((char)data_in->pulseRateCorrectedBuf[0]);
        tft.print((char)data_in->pulseRateCorrectedBuf[1]);
        tft.print((char)data_in->pulseRateCorrectedBuf[2]);
        tft.print(" BPM                  |");
        tft.setTextColor(GREEN);
        tft.print("|                                      |");
      }
    }
    
    if (respFlag) {
      // Find cursor location
      if (tempFlag && pressFlag && pulseFlag) {
        tft.setCursor(0,88);
      } else if ((tempFlag && pressFlag) || (pressFlag && pulseFlag)) {
        tft.setCursor(0, 72);
      } else if (tempFlag && pulseFlag) {
        tft.setCursor(0, 56);
      } else if (pressFlag) {
        tft.setCursor(0, 56);
      } else if (tempFlag || pulseFlag) {
        tft.setCursor(0, 40);
      } else {
        tft.setCursor(0, 24);
      }
      
      if (rrOutOfRange == 1) {
          if ((rrHigh || rrLow) && alarmAcknowledge == 0) {
          tft.setTextColor(RED);
        } else {
          tft.setTextColor(ORANGE);
        }
      } else {
        tft.setTextColor(GREEN);
      }
      
      tft.print("| Respiration Rate: ");
      tft.print((char)data_in->respirationRateCorrectedBuf[0]);
      tft.print((char)data_in->respirationRateCorrectedBuf[1]);
      tft.print((char)data_in->respirationRateCorrectedBuf[2]);
      //tft.print((char)data_in->respirationRateCorrectedBuf[3]);
      tft.print(" BPM            |");
      tft.setTextColor(GREEN);
      tft.print("|                                      |");
      }
    // print battery status
    if (lowPower == TRUE) {
      tft.setTextColor(RED);
    } else {
      tft.setTextColor(GREEN);
    }
  
    unsigned char battery[3];
    intToChar(battery, (int)*(data_in->batteryState));
    
    if (tempFlag && pressFlag && pulseFlag && respFlag) {
      tft.setCursor(0, 104);
    } else if ( (tempFlag && pressFlag && pulseFlag) || (tempFlag && pressFlag && respFlag) || (pulseFlag && pressFlag && respFlag) ) {
      tft.setCursor(0,88);
    } else if ( tempFlag && pulseFlag && respFlag ) {
      tft.setCursor(0, 72);
    } else if ((tempFlag && pressFlag) || (pulseFlag && pressFlag) || (respFlag && pressFlag)) {
      tft.setCursor(0, 72);
    } else if ((tempFlag && pulseFlag) || (tempFlag && respFlag) || (respFlag && pulseFlag)) {
      tft.setCursor(0, 56);
    } else if (pressFlag) {
      tft.setCursor(0, 56);
    } else if (tempFlag || pulseFlag || respFlag) {
      tft.setCursor(0, 40);
    } else {
      tft.setCursor(0, 24);
    } 
    
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
  
    // Acknowledge area
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.setCursor(50, 227);
    if (alarmAcknowledge != 0) {
      tft.fillRect(0, 216, 250, 60, GREEN);
    } else {
      tft.fillRect(0, 216, 250, 60, RED);
    }
    tft.print("Acknowledge");
  
    // Back area
    tft.setTextSize(4);
    tft.setCursor(70, 285);
    tft.fillRect(0, 270, 250, 60, LPURPLE);
    tft.print("BACK");
  
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.setCursor(0, 0);
    
  } else if (cur == DIS){
    drawDis(data);
  }
}

void warningAlarm (void* data) {
  WarningAlarmData* data_in = (WarningAlarmData*) data;
  int tempFixed = (int)*(data_in->temperatureRawBuf);
   tempFixed = 30 + tempFixed / 10;
  int systoFixed = (int) *(data_in->bloodPressRawBuf);
  systoFixed = 92 + systoFixed * 34 / 115;
  int diasFixed = (int) *(data_in->bloodPressRawBuf + 8);
  diasFixed = 50 + diasFixed * 26/115;
  int pulseFixed = (int) *(data_in->pulseRateRawBuf);
  pulseFixed = 116 / 3 + pulseFixed * 37 / 15;
  int respFixed = (int) *(data_in->respirationRateRawBuf);

  // Set Temperature flag
  if (tempFixed < 34.295 || tempFixed > 39.69) {
    tempOutOfRange = 1;    
    if (tempFixed > 43.47) {
      tempHigh = TRUE;
    } else if (tempFixed < 30.685) {
       tempLow = TRUE;
    }
  } else {
    tempOutOfRange = 0;
    tempHigh = FALSE;
    tempLow = FALSE;
  }

  // Set BP flag
  if (systoFixed > 130 || systoFixed < 120 || diasFixed > 80 || diasFixed <70) {
    if (diasFixed < 70 || diasFixed > 80) {
      diasOutOfRange = 1;
      if (diasFixed < 56 || diasFixed > 96) {
        diasRed = TRUE;
      } else {
        diasRed = FALSE;
      }
    } else {
      diasOutOfRange = 0;
      alarmAcknowledge = 0;
      diasRed = FALSE;
    }
    
    if (systoFixed > 130 || systoFixed < 120) {
      sysOutOfRange = TRUE;
      if (systoFixed > 156 || systoFixed < 96) {
        sysRed = TRUE;
      } else {
        sysRed = FALSE;
      }
    } else {
      alarmAcknowledge = 0;
      sysOutOfRange = FALSE;
      sysRed = FALSE;
    }
  } else {
    alarmAcknowledge = 0;
    sysOutOfRange = 0;
    diasOutOfRange = 0;
  }

  // set Pulse Rate flag
  if (pulseFixed < 57  || pulseFixed > 105) {
    pulseOutOfRange = 1;
     if (pulseFixed < 51) {
      pulseLow = TRUE;
    } else if (pulseFixed  > 115) {
      pulseHigh = TRUE;
    }
  } else {
    pulseOutOfRange = 0;
    pulseLow = FALSE;
    pulseHigh = FALSE;
  }

  // Set respiration rate flag
  if (respFixed < 11 || respFixed > 26) {
    rrOutOfRange = 1;
    if (respFixed > 28) {
      rrHigh = TRUE;
    } else {
      rrHigh = FALSE;
    }
    if (respFixed < 11) {
      rrLow = TRUE;
    } else {
      rrLow = FALSE;
    }
  }

  // Set Battery Flags
  if (*(data_in->batteryState) < 20  ) {
    lowPower = TRUE;
  } else {
    lowPower = FALSE;
  }
}

// Genertate status data
void statusF (void* data) {
  //Serial.println("dsf");
  Status* data_in = (Status*) data;
  if (data_in->batteryState > 0) {
    *(data_in->batteryState) -= 1;
  }
}

int tempOrange = 1;
unsigned long prevTemp = 0;

int sysOrange = 1;
unsigned long prevSys = 0;

int diasOrange = 1;
unsigned long prevDias = 0;

int pulseOrange = 1;
unsigned long prevPulse = 0;

int tempPrinted = 0;
int sysPrinted = 0;
int diasPrinted = 0;

void flash(void* data) {
  if (cur == ANNUN) { 
    DisplayData* data_in = (DisplayData*) data;
    
    int place = 24;
    if (tempFlag) {
      if (tempOutOfRange && !((tempHigh || tempLow) && alarmAcknowledge == 0)) {
          tft.setCursor(0, place); 
          if (tempOrange == 1) {
            tft.setTextColor(ORANGE);
            tft.print("| Temperature: ");
            tft.print((char)data_in->tempCorrectedBuf[1]);
            tft.print((char)data_in->tempCorrectedBuf[2]);
            tft.print(" C                    |");
            tft.setTextColor(GREEN);
            tft.print("|                                      |");
          } else {
            tft.fillRect(0, place, 250, 8, BLACK);
          }
          if (timer - prevTemp >= 2) {
            prevTemp = timer;
            tempOrange = -(tempOrange);
          }
      } 
    }
    if (tempFlag) {
      place = place + 16;
    }
    if (pressFlag) {
      if ((sysOutOfRange == 1 && !sysRed) ||  (sysRed == 1 && alarmAcknowledge != 0)) {
          tft.setCursor(0, place);
          if (sysOrange == 1) {
            tft.setTextColor(ORANGE);
            tft.print("| Systolic Pressure: ");
            tft.print((char)data_in->bloodPressCorrectedBuf[0]);
            tft.print((char)data_in->bloodPressCorrectedBuf[1]);
            tft.print((char)data_in->bloodPressCorrectedBuf[2]);
            tft.print(" mmHg          |");
            tft.setTextColor(GREEN);
            tft.print("|                                      |");
          } else {
            tft.fillRect(0, place, 250, 8, BLACK);
          }
          if (timer - prevSys >= 1) {
            prevSys = timer;
            sysOrange = -(sysOrange);
          }
      }
    }
    if (pressFlag) {
      place = place + 16;
    }

    if (pressFlag) {
      if ((diasOutOfRange == 1 && !diasRed) ||  (diasRed == 1 && alarmAcknowledge != 0)) {
        tft.setCursor(0, place);
        if (diasOrange == 1) {
          tft.setTextColor(ORANGE);
          tft.print("| Diastolic Pressure: ");
          tft.print((char)data_in->bloodPressCorrectedBuf[8]);
          tft.print((char)data_in->bloodPressCorrectedBuf[9]);
          tft.print((char)data_in->bloodPressCorrectedBuf[10]);
          tft.print(" mmHg         |");
          tft.setTextColor(GREEN);
          tft.print("|                                      |"); 
        } else {
          tft.fillRect(0, place, 250, 8, BLACK);
        }
        if (timer - prevDias >= 1) {
          prevDias = timer;
          diasOrange = -(diasOrange);
        }
      }
    }
    
    if (pressFlag) {
      place = place + 16;
    }

    if (pulseFlag) {
      if (pulseOutOfRange == 1) {
          if(!((pulseLow || pulseHigh) && alarmAcknowledge == 0)) {
             tft.setCursor(0, place);
            if (pulseOrange == 1) {
              tft.setTextColor(ORANGE);
              tft.print("| Pulse Rate: ");
              tft.print((char)data_in->pulseRateCorrectedBuf[0]);
              tft.print((char)data_in->pulseRateCorrectedBuf[1]);
              tft.print((char)data_in->pulseRateCorrectedBuf[2]);
              tft.print(" BPM                  |");
              tft.setTextColor(GREEN);
              tft.print("|                                      |");
            } else {
               tft.fillRect(0, place, 250, 8, BLACK);
            }
            if (timer - prevPulse >= 4) {
              prevPulse = timer;
              pulseOrange = -(pulseOrange);
            }
          }
      }
    }
  }
}


// indicating # of taskes to issue
int issue_count = 0;

void issue(unsigned long* interval, unsigned long* prev_time, unsigned long period, TCB* block, Bool* flag, int use_flag) {
  if (use_flag) {
    if (*flag == TRUE) {
      insert(block);
      *flag = FALSE;
      issue_count += 1;
    }
  } else {
    if ((*interval == 0 && *prev_time == period) || measureFlag == TRUE) {
      *flag = FALSE;
      insert(block);
      issue_count += 1;
    }
  }
}

unsigned long prev_b;

void scheduler() {
  head = NULL;
  tail = NULL;
  insert(&co);
  insert(&key);
  Bool updataFlag = FALSE;
  issue_count = 2;
  if (cur < 0||cur > 5) {
    Serial.println(cur);
    cur = 2;
  }
  if (cur == ANNUN || cur == DIS) {
    if (prev == cur) {
      interval = timer - start;
    }
    issue(&interval, &timer_prev, 19, &EKG_capture, &EKGCapFlag, 1);
    issue(&interval, &timer_prev, 19, &meas, &measureFlag, 0);
    issue(&interval, &timer_prev, 19, &EKG_process, &EKGProFlag, 1);
    issue(&interval, &timer_prev, 19, &comp, &computeFlag, 1);
    issue(&interval, &timer_prev, 0, &warn, &warnFlag, 0);
    issue(&interval, &timer_prev, 19, &disp, &disFlag, 1);
    if (dataUp || warnUp)
      updataFlag = TRUE;
    issue(&interval, &timer_prev, 19, &reCOM, &updataFlag, 0);
    timer_prev = interval;
  } 
  unsigned long block = (timer-start) % 20;
  issue(&block, &prev_b, 19, &stat, NULL, 0);
  insert(&reDispl);
  issue_count++;
  prev_b = block;
  TCB* cur = head;
  int i;

  for (i = 0; i < issue_count; i++) {
    TCB* p = cur;
    (*cur->mytask)(cur->taskDataPr);
    cur = cur->next;
  }
}
#endif
