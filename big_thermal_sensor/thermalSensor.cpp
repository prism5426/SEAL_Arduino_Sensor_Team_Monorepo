#include "thermalSensor.h"

void updateThermalData(float* pixels[AMG_COLS * AMG_ROWS], Adafruit_AMG88xx* amg) {
    //read all the pixels
    (*amg).readPixels(*pixels);
}

void thermalSensorTask(void* thData) {
    thermalSensorData* data = (thermalSensorData*) thData;

    updateThermalData(data->pixels, data->amg);
}
