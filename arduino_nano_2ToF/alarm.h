#pragma once

#include "thermalSensor.h"
#include "tof.h"

const int ALARM_LED = PD7; // 
#define OFFSET 12
#define HUMAN_TEMP 37

// timer states
typedef enum {
    TIMER_STATE_HALT,
    TIMER_STATE_START  
} TIMER_STATE;

typedef struct alarmData {
    tofSensorData* tofData;
    bool* alarmStatus;
    TIMER_STATE* state; 
    uint16_t* blinkRate;
    float* MAX_TEMP;
    float* pixels[AMG_COLS * AMG_ROWS];
} alarmData;

void alarmTask(void* aData);
void timer_init();
void start_timer();
void stop_timer();
uint16_t RoundUp(uint16_t toRound);
