#include "ultrasonic.h"

void updateDistance(float* distance) {
    // generate a 10us high pulse to drive the trig pin
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // echo pin
    long duration = pulseIn(ECHO_PIN, HIGH);
    
    *distance = duration*0.034/2;
}

void thermoCamTrigger(float* distance, bool* thermalCam) {
   *thermalCam = *distance <= 200 ? 1 : 0;
}

void ultrasonicTask(void* usData) {
    ultrasonicData* data = (ultrasonicData*) usData;

    updateDistance(data->distance);
    thermoCamTrigger(data->distance, data->thermalCam);
}
