#include "big_thermal_sensor.h"

// tft driver
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Task Control Blocks
TCB thermalSensorTCB;

TCB* head = NULL;
TCB* taskPtr = NULL;
TCB* tail = NULL;

// thermal sensor task data
Adafruit_AMG88xx amg;
thermalSensorData thData;
float pixels[AMG_COLS * AMG_ROWS];


// timebaseflag
volatile int timeBaseFlag;
unsigned long timeIntv;

void setup() {
  // put your setup code here, to run once:
  // setup interrupt
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerISR);
   
  // Initialize Display
  tft.reset();
  pinMode(XM, OUTPUT);
  pinMode(YM, OUTPUT);

  // Initialize thermal sensor
  thermal_sensor_setup();
  thData = {&amg, {0}};
  thermalSensorTCB.task         = &thermalSensorTask;
  thermalSensorTCB.taskDataPtr  = &thData;
  thermalSensorTCB.next         = NULL;
  thermalSensorTCB.prev         = NULL;

  // initialize tasks
  insertTask(&thermalSensorTCB);
  
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
  

  // Initialize serial communication
  Serial.begin(9600);
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

void loop() {
  // put your main code here, to run repeatedly:
  if (1 == timeBaseFlag) {
      timeBaseFlag = 0;  
  }
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
        if (head == tail) { // TODO check to make sure only node is deleted
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

void timerISR() {
    timeBaseFlag = 1;
}
