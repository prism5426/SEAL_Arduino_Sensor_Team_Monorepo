#pragma once

#include "thermalSensor.h"
#include "Display.h"
#include "ultrasonic.h"

const int ALARM_LED = PA2; // pin 24
#define OFFSET 12
#define HUMAN_TEMP 37

typedef enum {
    TIMER_STATE_HALT,
    TIMER_STATE_START  
} TIMER_STATE;

typedef struct alarmData {
    ultrasonicData* usData; 
    bool* alarmStatus;
    TIMER_STATE* state; 
    uint16_t* blinkRate;
    float* pixels[AMG_COLS * AMG_ROWS];
    
} alarmData;

void alarmTask(void* aData);
void timer_init();
void start_timer();
void stop_timer();
uint16_t RoundUp(uint16_t toRound);
