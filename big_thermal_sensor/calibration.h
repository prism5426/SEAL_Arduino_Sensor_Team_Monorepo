/*header file for calibration task
 * 
 */

#pragma once

#include "thermalSensor.h"
#include "tof.h"

typedef struct calibrationData {
   float* distance;
   float* pixels[AMG_COLS * AMG_ROWS];
   float* MIN_TEMP;
} clibrationData;

void calibrationTask(void* cData);
void updateCalibration(float* distance, float* pixels[AMG_COLS * AMG_ROWS], float* MIN_TEMP, int mode);
