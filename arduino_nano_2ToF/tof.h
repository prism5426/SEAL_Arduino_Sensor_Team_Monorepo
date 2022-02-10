#pragma once

//#include <SparkFun_VL53L1X.h>
#include <Adafruit_VL53L1X.h>

typedef struct tofSensorData {
  //SFEVL53L1X* tof;
  Adafruit_VL53L1X* tof;
  int16_t* distance;
  bool* thermalCam;     
  float* MAX_TEMP;
} tofSensorData;

void updateDistance(Adafruit_VL53L1X* sensor, int16_t* distance, bool* thermalCam);
void tofSensorInit(Adafruit_VL53L1X*);
void tofSensorTask(void*);
void calibrate(float *MAX_TEMP, int16_t* distance);
