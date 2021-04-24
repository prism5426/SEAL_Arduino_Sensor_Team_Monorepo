#include "ultrasonic.h"

void updateDistance(float* distance) {
    noInterrupts();
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    *distance = duration*0.034/2;
    interrupts();
}

void ultrasonicTask(void* usData) {
    ultrasonicData* data = (ultrasonicData*) usData;

    updateDistance(data->distance);
}
