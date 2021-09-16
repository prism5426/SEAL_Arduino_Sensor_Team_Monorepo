#include <ComponentObject.h>
#include <RangeSensor.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <avr/sleep.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#include "TaskControlBlock.h"
#include "display.h" // this header file includes "thermalSensor.h"
#include "alarm.h"
#include "calibration.h"
#include "tof.h"

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

const uint8_t INTERRUPT_PIN = 19;
const int Status_LED = 37;
const int buttonPin = 41;

//function declaration
void insertTask(TCB* node);
void deleteTask(TCB* node);
void scheduler();

void thermal_sensor_setup();
void tofSensorInit();
void print_pixels();
void print_distance();
void timerISR();
