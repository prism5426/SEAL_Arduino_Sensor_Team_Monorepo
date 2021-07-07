#pragma once

#include <VL53L1X.h>

typedef struct tofSensorData {
  VL53L1X* tof;
  float* distance;
  bool* thermalCam;     
} tofSensorData;

void updateDistance(VL53L1X* sensor, float* distance, bool* thermalCam);
void tofSensorInit(VL53L1X*);
void tofSensorTask(void*);
