#include "structures_and_taskes.h"

void measure (void* data) {
	MeasureData* data_in = (MeasureData*) data;
	if (times % 2 == 0) {
		if (data_in->tempretureRaw < 50 && data_in->tempretureRaw >= 15) {
			data_in->tempretureRaw += 2;
		} else if (data_in->tempretureRaw >= 50) {
			data_in->tempretureRaw -= 2;
		}
		if (data_in->systolicPressRaw < 100
	} else {
	
	}		
}

void compute (void* data) {
	ComputeData* data_in = (ComputeData*) data;
	*(data_in->tempCorrected) = 5 + 0.75 * data_in->temperatureRaw;
	*(data_in->sysCorrected) = 9 + 2 * data_in->systolicPressRaw;
	*(data_in->diasCorrected) = 6 + 1.5 * data_in->diastolicPressRaw;
	*(data_in->prCorrected) = 8 + 3 * data_in->pulseRateRaw;	
}