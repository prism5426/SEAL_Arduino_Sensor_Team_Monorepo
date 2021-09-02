#include "tof.h"

void updateDistance(VL53L1X* sensor, float* distance, bool* thermalCam) {
   // sensor reads in "mm", distance in "cm"
   *distance = (*sensor).read() / 10;
   *thermalCam = *distance <= 200 ? 1 : 0;
   Serial.print("distance = ");
   Serial.println(*distance);
}

void tofSensorTask(void* tofData){
  tofSensorData* data = (tofSensorData*)tofData;
  updateDistance(data->tof, data->distance, data->thermalCam);
}
