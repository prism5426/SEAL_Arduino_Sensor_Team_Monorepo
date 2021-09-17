#include "calibration.h"

void updateCalibration(float* distance, float* pixels[AMG_COLS * AMG_ROWS],float* TOTAL_MIN_TEMP, float* MIN_TEMP, int* counter, bool* UPDATE_THRESHOLD, int mode) {
    if (mode == 0) { // calibrate based on current min temperature
        float min_value = (*pixels)[0];
        for (int i = 1; i < 64; i++) {
          if ((*pixels)[i] < min_value) {
            min_value = (*pixels)[i];
          }
        }
        if (*distance > 0.5){
          *TOTAL_MIN_TEMP += min_value;
          *counter ++;
        }
        if (*UPDATE_THRESHOLD) {
          *MIN_TEMP = *TOTAL_MIN_TEMP / *counter;
          *counter = 0;
          *TOTAL_MIN_TEMP = 0;
          *UPDATE_THRESHOLD = 0;
        }
        return;
    } else if (mode == 1) { // calibrate based on avg temperature, assume no hotspot in frame
        double avg_value = (*pixels)[0];
        for (int i = 1; i < 64; i++) {
            avg_value += (*pixels)[i];
        }
        avg_value /= 64;
    }
}

// calibration task
void calibrationTask(void* cData) {
    calibrationData* data = (calibrationData*)cData;
    // TODO: change mode to enum
   updateCalibration(data->distance, data->pixels, data->TOTAL_MIN_TEMP, data->MIN_TEMP, data->counter, data->UPDATE_THRESHOLD, 0);  
}
