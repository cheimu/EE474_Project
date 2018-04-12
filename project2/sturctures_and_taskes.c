#include "structures_and_taskes.h"
enum_myBool { EVEN = 0, ODD = 1 };
typedef enum_myBool Bool;

volatile static Bool times;

void measure (void* data) {
	MeasureData* data_in = (MeasureData*) data;
	
	switch(times) {
		case EVEN :
			// temperatureRaw
			if (data_in->tempretureRaw < 50 && data_in->tempretureRaw >= 15) {
				data_in->tempretureRaw += 2;
			} else if (data_in->tempretureRaw < 15) {
				data_in->tempretureRaw -= 2;
			}
			
			// systolicPressRaw or diastolicPressRaw
			if (data_in->systolicPressRaw <= 100) {
					data_in->systolicPressRaw+=3;
			}else if (data_in->systolicPressRaw > 100 && data_in->diastolicPressRaw < 40 )) {
				data_in->systolicPressRaw = 80;
				data_in->diastolictolicPressRaw = 80;
			} else if (data_in->systolicPressRaw > 100 && data_in->diastolicPressRaw >= 40) {
				data_in->diastolicPressRaw -= 2;	
			}
			
			// pulseRateRaw
			if (data_in->pulseRateRaw < 40 && data_in->pulseRateRaw >= 15) {
				data_in->pulseRateRaw--;
			} else if (data_in->tempretureRaw < 15) {
				data_in->tempretureRaw++;
			}
			times = ODD;
			break;
		case ODD  :
			if (data_in->tempretureRaw < 50 && data_in->tempretureRaw >= 15) {
				data_in->tempretureRaw--;
			} else if (data_in->tempretureRaw < 15) {
				data_in->tempretureRaw++;
			}
			
			// systolicPressRaw or diastolicPressRaw
			if (data_in->systolicPressRaw <= 100) {
					data_in->systolicPressRaw--;
			}else if (data_in->systolicPressRaw > 100 && data_in->diastolicPressRaw < 40 )) {
				data_in->systolicPressRaw = 80;
				data_in->diastolictolicPressRaw = 80;
			} else if (data_in->systolicPressRaw > 100 && data_in->diastolicPressRaw >= 40) {
				data_in->diastolicPressRaw++;	
			}
			
			// pulseRateRaw
			if (data_in->pulseRateRaw < 40 && data_in->pulseRateRaw >= 15) {
				data_in->pulseRateRaw += 3;
			} else if (data_in->pulseRateRaw < 15) {
				data_in->pulseRateRaw -= 3;
			}
			times = EVEN;
			break;
	}
	
}


void compute (void* data) {
	ComputeData* data_in = (ComputeData*) data;
	*(data_in->tempCorrected) = 5 + 0.75 * data_in->temperatureRaw;
	*(data_in->sysCorrected) = 9 + 2 * data_in->systolicPressRaw;
	*(data_in->diasCorrected) = 6 + 1.5 * data_in->diastolicPressRaw;
	*(data_in->prCorrected) = 8 + 3 * data_in->pulseRateRaw;	
}

void dispaly (void* data) {
	DisplayData* data_in = (DisplayData*) data;
	printf("Temperature: %sC", data->tempCorrected);
	printf("Temperature: %smm Hg", data->sysPressCorrected);
	printf("Temperature: %smm Hg", data->diastolicPressCorrected);
	printf("Temperature: %sBPM", data->pulseRateCorrected);
	printf("Temperature: %hu%%", data->batteryState);
}

void warningAlarm (void* data) {
	WarningAlarmData* data_in = (WarningAlarmData*) data;
	if (data_in->temperatureRaw < 36.1 || data_in->temperatureRaw > 37.8) {
		printf("You are Dead");

	} else {
		printf("(- w -)");

	}
	if (data_in->systolicPressRaw != 120) {
		printf("You are Dead");
		
	} else {
		printf("(- w -)");
		
	}
	if (data_in->diastolicPressRaw != 80) {
		printf("You are Dead");
		
	} else {
		printf("(- w -)");
		
	}
	if (data_in->pulseRateRaw < 60  || data_in->pulseRateRaw > 100  ) {
		printf("You are Dead");
		
	} else {
		printf("(- w -)");
		
	}
	
	if (data_in->batteryState < 20  ) {
		printf("You are Dead");
		
	} else {
		printf("(- w -)");
		
	}
	
	
}

void status (void* data) {
	Status* data_in = (Status*) data;
	data_in->batteryState--;
}