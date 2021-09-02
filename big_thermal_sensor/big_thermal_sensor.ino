#include "big_thermal_sensor.h"

#define PIXEL_DEBUG 0
#define DIST_DEBUG 0

// tft driver
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Task Control Blocks
TCB thermalSensorTCB;
TCB displayTCB;
TCB touchInputTCB;
TCB alarmTCB;
TCB tofTCB;

TCB* head = NULL;
TCB* taskPtr = NULL;
TCB* tail = NULL;

// display task data
displayTaskData dData;

// display history data
displayHistory dhData;

// thermal sensor task data
Adafruit_AMG88xx amg;
thermalSensorData thData;
float pixels[AMG_COLS * AMG_ROWS];
float HDTemp[HD_ROWS * HD_COLS];   

bool thermalCam = 0;
bool prev_thermalCam;

// alarm task data
alarmData aData;
bool alarmStatus;
TIMER_STATE state = TIMER_STATE_HALT;
uint16_t blinkRate = 0;

// tof task data
tofSensorData tofData;
VL53L1X tof;
float distance;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
   
  // Initialize Display and displayHistory
  tft.reset();
  pinMode(XM, OUTPUT);
  pinMode(YM, OUTPUT);
  dhData                        = {&prev_thermalCam};
  dData                         = {&tft, &dhData, &thData};
  displayTCB.task               = &displayTask;
  displayTCB.taskDataPtr        = &dData;
  displayTCB.next               = NULL;
  displayTCB.prev               = NULL;
  

  // Initialize thermal sensor
  thermal_sensor_setup();
  thData                        = {&thermalCam, &amg, pixels, HDTemp};
  thermalSensorTCB.task         = &thermalSensorTask;
  thermalSensorTCB.taskDataPtr  = &thData;
  thermalSensorTCB.next         = NULL;
  thermalSensorTCB.prev         = NULL;

  // Initialize ToF 
  tofSensorInit();
  tofData                       = {&tof, &distance, &thermalCam};
  tofTCB.task                   = &tofSensorTask;
  tofTCB.taskDataPtr            = &tofData;
  tofTCB.next                   = NULL;
  tofTCB.prev                   = NULL;

  // Initialize alarm led
  timer_init();
  aData                         = {&tofData, &alarmStatus, &state, &blinkRate, pixels};
  alarmTCB.task                 = &alarmTask;
  alarmTCB.taskDataPtr          = &aData;
  alarmTCB.next                 = NULL;
  alarmTCB.prev                 = NULL;

  // initialize tasks
  insertTask(&displayTCB);
  insertTask(&thermalSensorTCB);
  insertTask(&alarmTCB);
  insertTask(&tofTCB);
  
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
    identifier=0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, float-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
}

  tft.begin(identifier);

  // initialize black screen
  tft.fillScreen(BLACK);  
  
}

void thermal_sensor_setup() {
    Serial.println(F("AMG88xx test"));

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Thermistor Test --");

    Serial.println();

    delay(100); // let sensor boot up
}

void tofSensorInit() {
  tof.setTimeout(500);
  if(!tof.init()) {
    Serial.println("Failed to detect or initialize ToF sensor");
    while(1);  
  }
  
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  tof.setDistanceMode(VL53L1X::Long);
  tof.setMeasurementTimingBudget(50000);  

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  tof.startContinuous(50);
}

void loop() {
    scheduler();
    if (PIXEL_DEBUG) print_pixels();
    if (DIST_DEBUG) print_distance();
}

void insertTask(TCB* node) {
    node -> next = NULL; // reset to NULL b/c we didnt reset when deleting
    node -> prev = NULL;
    if (NULL == head) { // If the head pointer is pointing to nothing
        head = node; // set the head and tail pointers to point to this node
        tail = node;
    } else { // otherwise, head is not NULL, add the node to the end of the list
        tail -> next = node;
        node -> prev = tail; // note that the tail pointer is still pointing
        // to the prior last node at this point
        tail = node; // update the tail pointer
        tail -> next = NULL;
    }
    return;
}

void deleteTask(TCB* node){
    if (NULL != head) {
        if (head == tail) { 
            head = NULL;
            tail = NULL;
        } else if (head == node) {
            head = head -> next;
            head -> prev = NULL;
        } else if (tail == node) {
            tail = tail -> prev;
            tail -> next = NULL;
        } else {
            //middle
            (node -> next) -> prev = node -> prev;
            (node -> prev) -> next = node -> next;
        }
    }
    return;
}

void scheduler() {
    while (taskPtr) {
        taskPtr->task(taskPtr->taskDataPtr);
        taskPtr = taskPtr->next;
    }
    taskPtr = head;
}

void print_pixels() {
    Serial.print("pixels: [");
    for (int i = 0; i < AMG_COLS * AMG_ROWS; i++) {  
        Serial.print(pixels[i]);
        if (i == AMG_COLS * AMG_ROWS - 1) Serial.println("]");
        else Serial.print(", ");
    } 
    Serial.println("]"); 
}

void print_distance() {
    Serial.print("distance = ");
    Serial.println(distance);  
}
