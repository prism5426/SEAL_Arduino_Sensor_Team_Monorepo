#include "Display.h"

void displayThermalArray(Elegoo_TFTLCD tft, void* thData) {
    thermalSensorData* data = (thermalSensorData*) thData;
    tft.setCursor(0, 0);
    tft.print("hello");
}

void update_displayThermalArray(Elegoo_TFTLCD tft, void* thData, void* dhData) {
   thermalSensorData* data = (thermalSensorData*) thData;  
}

void displayTask(void* dData) {
   displayTaskData* data = (displayTaskData*) dData;
   displayHistory* dhData = data->dhData;
   
   displayThermalArray(*(data->tft), data->thData);
}
