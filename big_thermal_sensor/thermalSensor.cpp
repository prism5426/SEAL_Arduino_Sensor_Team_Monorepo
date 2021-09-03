
#include "thermalSensor.h"

// variables for row/column interpolation
byte i, j, k, row, col, incr;
float intPoint, val, a, b, c, d, ii;
byte aLow, aHigh;

void updateThermalData(float* pixels[AMG_COLS * AMG_ROWS], Adafruit_AMG88xx* amg) {
    //read all the pixels
    (*amg).readPixels(*pixels);
}

void interpolate(float* pixels[AMG_COLS * AMG_ROWS], float HDTemp[HD_ROWS][HD_COLS]) {
   
}

// TODO: add color interpolation
void thermalSensorTask(void* thData) {
    thermalSensorData* data = (thermalSensorData*) thData;
//    if (*(data->thermalCam)) {
//       updateThermalData(data->pixels, data->amg);
    //interpolate(data->pixels, data->HDTemp);
//    }
    updateThermalData(data->pixels, data->amg);
}
