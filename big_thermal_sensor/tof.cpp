#include "tof.h"

void updateDistance(SFEVL53L1X *sensor, float *distance, bool *thermalCam)
{
    while (!(*sensor).checkForDataReady())
    {
        delay(1);
    }
    // sensor reads in "mm", distance in "mm"
    *distance = (*sensor).getDistance();
    *thermalCam = *distance <= 1830 ? 1 : 0; // turn on when distance less than 1830mm (~6ft)
}

void tofSensorTask(void* tofData){
  tofSensorData* data = (tofSensorData*)tofData;
  updateDistance(data->tof, data->distance, data->thermalCam);
}
