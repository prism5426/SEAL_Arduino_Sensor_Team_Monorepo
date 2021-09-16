#include "calibration.h"

void updateCalibration(float* distance, float* pixels[AMG_COLS * AMG_ROWS], float* MIN_TEMP, int mode) {
    if (mode == 0) { // calibrate based on current min temperature
        float min_value = (*pixels)[0];
        for (int i = 1; i < 64; i++) {
          if ((*pixels)[i] < min_value) {
            min_value = (*pixels)[i];
          }
        }
        *MIN_TEMP = min_value;
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
    updateCalibration(data->distance, data->pixels, data->MIN_TEMP, 0);  
}
