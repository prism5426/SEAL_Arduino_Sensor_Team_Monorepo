#include <Arduino.h>

#define TRIG_PIN 22
#define ECHO_PIN 23

typedef struct ultrasonicData {
  float* distance;
} ultrasonicData;

void ultrasonicTask(void*);
