#include <Adafruit_VL53L1X.h>
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <vl53l1x_class.h>
#include <vl53l1x_error_codes.h>

#include <ComponentObject.h>
#include <avr/sleep.h>
#include "big_thermal_sensor.h"

#define PIXEL_DEBUG 0
#define DIST_DEBUG 0

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 18

//Task Control Blocks
TCB thermalSensorTCB;
TCB alarmTCB;
TCB tofTCB;
TCB tofTCB2;

TCB *head = NULL;
TCB *taskPtr = NULL;
TCB *tail = NULL;

// thermal sensor task data
Adafruit_AMG88xx amg;
thermalSensorData thData;
float pixels[AMG_COLS * AMG_ROWS];
float MIN_TEMP = 25;
float MAX_TEMP;
bool thermalCam = 0;

// alarm task data
alarmData aData;
bool alarmStatus;
TIMER_STATE state = TIMER_STATE_HALT;
uint16_t blinkRate = 0;

// tof task data
tofSensorData tofData, tofData2;
int16_t distance, distance2;
Adafruit_VL53L1X distanceSensor;

void setup(void)
{
  // Initialize ToF
  pinMode(7, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Wire.begin();
  
  //Serial.println(distanceSensor.begin());
  if (! distanceSensor.begin(0x29, &Wire)) // Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    Serial.println(distanceSensor.vl_status);
    while (1)       delay(10); 
  }
  
  Serial.println(F("VL53L1X sensor OK!"));

  if (! distanceSensor.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(distanceSensor.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));
  
  // 8ft is 2438.4 mm -> rounded to 2500mm
  //distanceSensor.startRanging(); // Write configuration bytes to initiate measurement

  // Initialize thermal sensor
  thermal_sensor_setup();
  thData = {&thermalCam, &amg, pixels};
  thermalSensorTCB.task = &thermalSensorTask;
  thermalSensorTCB.taskDataPtr = &thData;
  thermalSensorTCB.next = NULL;
  thermalSensorTCB.prev = NULL;
  //Serial.println(F("Thermal camera init complete"));
  
  // Initialize ToF
  tofData = {&distanceSensor, &distance, &thermalCam, &MAX_TEMP};
  tofTCB.task = &tofSensorTask;
  tofTCB.taskDataPtr = &tofData;
  tofTCB.next = NULL;
  tofTCB.prev = NULL;
  //Serial.println(F("ToF1 init complete"));

  // Initialize ToF2
  tofData2 = {&distanceSensor, &distance, &thermalCam, &MAX_TEMP};
  tofTCB2.task = &tofSensorTask;
  tofTCB2.taskDataPtr = &tofData2;
  tofTCB2.next = NULL;
  tofTCB2.prev = NULL;
  //Serial.println(F("ToF2 init compelte"));

  // Initialize alarm led
  timer_init();
  aData = {&tofData, &alarmStatus, &state, &blinkRate, &MAX_TEMP, pixels};
  alarmTCB.task = &alarmTask;
  alarmTCB.taskDataPtr = &aData;
  alarmTCB.next = NULL;
  alarmTCB.prev = NULL;
  //Serial.println(F("LED init complete"));

  // initialize tasks
//  insertTask(&displayTCB);
  insertTask(&thermalSensorTCB);
  insertTask(&alarmTCB);
  insertTask(&tofTCB);
  insertTask(&tofTCB2);
  //Serial.println(F("ALL INIT COMPLETE"));
}

void thermal_sensor_setup()
{
  //    Serial.println(F("AMG88xx test"));
  bool status;
  
  // default settings
  status = amg.begin();
  if (!status)
  {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1)
      ;
  }
  //    Serial.println("-- Thermistor Test --");
  //    Serial.println();
  delay(100); // let sensor boot up
}

int counter;
float total_min_value;

void loop(void)
{ 
//  debugging
//  if (PIXEL_DEBUG) print_pixels();
//  if (DIST_DEBUG) print_distance(); 
    scheduler();
}

void insertTask(TCB *node)
{
  node->next = NULL; // reset to NULL b/c we didnt reset when deleting
  node->prev = NULL;
  if (NULL == head)
  { // If the head pointer is pointing to nothing
    head = node; // set the head and tail pointers to point to this node
    tail = node;
  }
  else
  { // otherwise, head is not NULL, add the node to the end of the list
    tail->next = node;
    node->prev = tail; // note that the tail pointer is still pointing
    // to the prior last node at this point
    tail = node; // update the tail pointer
    tail->next = NULL;
  }
  return;
}

void deleteTask(TCB *node)
{
  if (NULL != head)
  {
    if (head == tail)
    {
      head = NULL;
      tail = NULL;
    }
    else if (head == node)
    {
      head = head->next;
      head->prev = NULL;
    }
    else if (tail == node)
    {
      tail = tail->prev;
      tail->next = NULL;
    }
    else
    {
      //middle
      (node->next)->prev = node->prev;
      (node->prev)->next = node->next;
    }
  }
  return;
}

void scheduler()
{
  while (taskPtr)
  {
    taskPtr->task(taskPtr->taskDataPtr);
    taskPtr = taskPtr->next;
  }
  taskPtr = head;
}

void print_pixels()
{
  //  Serial.print("pixels: [");
  for (int i = 0; i < AMG_COLS * AMG_ROWS; i++)
  {
    Serial.print(pixels[i]);
    if (i == AMG_COLS * AMG_ROWS - 1)
      Serial.println("");
    else
      Serial.print(", ");
  }
  //  Serial.println("]");
}

void print_distance()
{
  Serial.print("distance = ");
  Serial.println(distance);

  /* 
   *  distance 2 not used
  Serial.print("distance2 = ");
  Serial.println(distance2);
  */
}
