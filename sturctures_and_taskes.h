#ifndef STRUCT
#define STRUCT

volatile static int times;

void measure(void* data);
void compute(void* data);
void display(void* data);
void warning_alarm(void* data);
void status (void* data);

typedef struct {
	unsigned int tempretureRaw;
	unsigned int systolicPressRaw;
	unsigned int distolicPressRaw;
	unsigned int pulseRateRaw;
} MeasureData;

typedef struct {
	unsigned int tempretureRaw;
	unsigned int systolicPressRaw;
	unsigned int distolicPressRaw;
	unsigned int pulseRateRaw;
	unsigned char* tempCorrected;
	unsigned char* sysPressCorrected;
	unsigned char* diastolicPressCorrected;
	unsigned char* pulseRateCorrected;
} ComputeData;

typedef struct {
	unsigned char* tempCorrected;
	unsigned char* sysPressCorrected;
	unsigned char* diastolicPressCorrected;
	unsigned char* pulseRateCorrected;
	unsigned short batteryState;
} DisplayData;

typedef struct {
	unsigned int tempretureRaw;
	unsigned int systolicPressRaw;
	unsigned int distolicPressRaw;
	unsigned int pulseRateRaw;
	unsigned short batteryState;
} WarningAlarmData;

typedef struct {
	unsigned short batteryState;
} Status;

typedef struct {
} SchedulerData;

typedef struct {
	void (*myTask)(void*);
	void* taskDataPr;
} TCB;
#endif