#ifndef DISPLAY_H_
#define DISPLAY_H_


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 10
#define MAXPRESSURE 1000

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define DISMAXX 240 // display max x value
#define DISMAXY 320 // display max y value
#define ButtonW 75  // button width
#define ButtonH 50  // button height
#define ButtonR 8   // button corner radius

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//low range of the sensor (this will be blue on the screen)
#define MINTEMP 20

//high range of the sensor (this will be red on the screen)
#define MAXTEMP 29

#define BOX_WIDTH 10
#define BOX_HEIGHT 10

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Elegoo_TFTLCD.h>

#include "TaskControlBlock.h"
#include "thermalSensor.h"

typedef struct displayHistory {
    bool* prev_thermalCam;
} displayHistory;

typedef struct displayTaskData {
    Elegoo_TFTLCD* tft;
    displayHistory* dhData;
    thermalSensorData* thData;
    
} displayTaskData;

void displayTask(void*);

#endif
