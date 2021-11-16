#include "Adafruit_AMG88xx.h"
#include <Wire.h>
#include "thermalSensor.h"
#include "TaskControlBlock.h"
#include "alarm.h"
#include "tof.h"

//function declaration
void insertTask(TCB* node);
void deleteTask(TCB* node);
void scheduler();

void thermal_sensor_setup();
void tofSensorInit();
void print_pixels();
void print_distance();
void timerISR();
