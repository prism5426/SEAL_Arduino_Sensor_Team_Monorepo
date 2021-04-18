#include "Display.h"

void displayThermalArray(Elegoo_TFTLCD tft, void* thData) {
    thermalSensorData* data = (thermalSensorData*) thData;
}

void update_displayThermalArray(Elegoo_TFTLCD tft, void* thData, void* hData) {
   thermalSensorData* data = (thermalSensorData*) thData;  
}

void displayTask(void* dData) {
   displayTaskData* data = (displayTaskdata*) dData;
   dispalyHistory hData = data->history;
}
