/*header file for calibration task
 * 
 */

#pragma once

#include "thermalSensor.h"
#include "tof.h"

typedef struct calibrationData {
   float* distance;
   float* MIN_TEMP;
   float* TOTAL_MIN_TEMP;
   int* counter;
   int* UPDATE_THRESHOLD;
  float* pixels[AMG_COLS * AMG_ROWS];
} calibrationData;

void calibrationTask(void* cData);
void updateCalibration(float* distance, float* pixels[AMG_COLS * AMG_ROWS],float* TOTAL_MIN_TEMP, float* MIN_TEMP, int* counter, int* UPDATE_THRESHOLD, int mode);
