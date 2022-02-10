#include "tof.h"
#include "vl53l1x_class.h"

void updateDistance(Adafruit_VL53L1X *sensor, int16_t *distance, bool *thermalCam)
{
    if ((*sensor).dataReady()) {
    // new measurement for the taking!
    *distance = (*sensor).distance();
    if (*distance == -1) {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println((*sensor).vl_status);
      return;
    }
    //Serial.print(F("Distance: "));
    //Serial.print(*distance);
    //Serial.println(" mm");

    // data is read out, time for another reading!
    (*sensor).clearInterrupt();
  }
}

void calibrate(float *MAX_TEMP, int16_t* distance) {
      // dynamic threshold code start
//      Serial.print(F("Distance: "));
//      Serial.print(*distance);
//      Serial.println(" mm");
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
