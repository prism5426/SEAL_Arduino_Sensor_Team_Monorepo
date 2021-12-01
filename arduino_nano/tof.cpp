#include "tof.h"

void updateDistance(SFEVL53L1X *sensor, float *distance, bool *thermalCam)
{
    while (!(*sensor).checkForDataReady())
    {
        delay(1);
    }
    // sensor reads in "mm", distance in "mm"
    *distance = (*sensor).getDistance();
    int addr = (*sensor).getI2CAddress();
    Serial.print("addr = ");
    Serial.println(addr);
    *thermalCam = *distance <= 1830 ? 1 : 0; // turn on when distance less than 1830mm (~6ft)
}

void calibrate(float *MAX_TEMP, float* distance) {
      // dynamic threshold code start
      if (*distance < 304.8) *MAX_TEMP = 27; // 1ft
      else if (*distance < 609) *MAX_TEMP = 26; //2ft
      else if (*distance < 914) *MAX_TEMP = 24; //3ft
      else if (*distance < 1219) *MAX_TEMP = 20; //4ft
      else if (*distance < 1524) *MAX_TEMP = 18; //5ft
      else if (*distance < 1828) *MAX_TEMP = 10; //6ft
//      else if (*distance > 6) *MAX_TEMP = 22; //7ft
      // dynamic threshold code end
}

void tofSensorTask(void* tofData){
  tofSensorData* data = (tofSensorData*)tofData;
  updateDistance(data->tof, data->distance, data->thermalCam);
  calibrate(data->MAX_TEMP, data->distance);
}
