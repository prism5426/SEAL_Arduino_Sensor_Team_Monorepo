
#include "Adafruit_AMG88xx.h"

#define AMG_COLS 8
#define AMG_ROWS 8

typedef struct thermalSensorData {
    Adafruit_AMG88xx* amg;
    float* pixels[AMG_COLS * AMG_ROWS];
} thermalSensorData;

void thermalSensorTask (void*);
