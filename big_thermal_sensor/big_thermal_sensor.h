#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Wire.h>
<<<<<<< HEAD
<<<<<<< HEAD
=======
#include <VL53L1X.h>
>>>>>>> parent of 6998423 (Merge pull request #1 from prism5426/updated-prototype)
=======
#include <VL53L1X.h>
>>>>>>> parent of 6998423 (Merge pull request #1 from prism5426/updated-prototype)
#include "TaskControlBlock.h"
#include "display.h" // this header file includes "thermalSensor.h"
#include "ultrasonic.h"
#include "alarm.h"

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin


//function declaration
void insertTask(TCB* node);
void deleteTask(TCB* node);
void scheduler();

void thermal_sensor_setup();
void print_pixels();
void print_distance();
void timerISR();
