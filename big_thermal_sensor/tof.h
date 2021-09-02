#pragma once

#include <SparkFun_VL53L1X.h>

typedef struct tofSensorData {
  SFEVL53L1X* tof;
  float* distance;
  bool* thermalCam;     
} tofSensorData;

void updateDistance(SFEVL53L1X* sensor, float* distance, bool* thermalCam);
void tofSensorInit(SFEVL53L1X*);
void tofSensorTask(void*);
