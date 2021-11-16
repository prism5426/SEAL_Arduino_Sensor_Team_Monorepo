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
tofSensorData tofData;
float distance;
SFEVL53L1X distanceSensor(Wire);

void setup(void)
{
  // Initialize ToF
  Wire.begin();
  pinMode(7, OUTPUT);
  
  Serial.begin(9600);

 Serial.println(distanceSensor.begin());
  if (distanceSensor.begin() != 0) // Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
      Serial.println("Sensor online!");
  // 8ft is 2438.4 mm -> rounded to 2500mm
  distanceSensor.setInterruptPolarityLow();
  distanceSensor.setDistanceModeLong();
  //    distanceSensor.setDistanceThreshold(10, 2500, 3);
  distanceSensor.startRanging(); // Write configuration bytes to initiate measurement

  // Initialize serial communication
//  Serial.begin(9600);
  Serial.print("starts here");

  // setup calibration
  //attachInterrupt(digitalPinToInterrupt(buttonPin),ISR, RISING);

  // Initialize thermal sensor
  thermal_sensor_setup();
  thData = {&thermalCam, &amg, pixels};
  thermalSensorTCB.task = &thermalSensorTask;
  thermalSensorTCB.taskDataPtr = &thData;
  thermalSensorTCB.next = NULL;
  thermalSensorTCB.prev = NULL;

  // Initialize ToF
  tofData = {&distanceSensor, &distance, &thermalCam, &MAX_TEMP};
  tofTCB.task = &tofSensorTask;
  tofTCB.taskDataPtr = &tofData;
  tofTCB.next = NULL;
  tofTCB.prev = NULL;

  // Initialize alarm led
  timer_init();
  aData = {&tofData, &alarmStatus, &state, &blinkRate, &MAX_TEMP, pixels};
  alarmTCB.task = &alarmTask;
  alarmTCB.taskDataPtr = &aData;
  alarmTCB.next = NULL;
  alarmTCB.prev = NULL;

  // initialize tasks
//  insertTask(&displayTCB);
  insertTask(&thermalSensorTCB);
  insertTask(&alarmTCB);
  insertTask(&tofTCB);
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

void tofSensorRoutine(float distance)
{
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  distanceSensor.clearInterrupt();
  // 6ft is 1828.8 mm -> rounded to 1830mm
  if (distance >= 1830)
  {
    //        Serial.println();
    //        Serial.println("No object within 6ft or 1830mm");
    distanceSensor.clearInterrupt();
    //        Going_To_Sleep();
  }
}

int counter;
float total_min_value;

void loop(void)
{ 
//  debugging
//  if (PIXEL_DEBUG) print_pixels();
//  if (DIST_DEBUG) print_distance(); 
//    Serial.println(distance);
//    Serial.print("max temp = ");
//    Serial.println(MAX_TEMP);
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
}

//void Going_To_Sleep()
//{
//  Serial.println(F("Entering sleep mode!"));
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//  sleep_enable();
//  noInterrupts();
//  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), wakeUp, FALLING);
//  digitalWrite(Status_LED, LOW); // Turn LED off
//  delay(1000);                  // Wait a second to allow the led to be turned off before going to sleep
//  Serial.flush();
//  interrupts();
//  sleep_cpu();
//}
//
//void wakeUp()
//{
//  Serial.println();
//  Serial.println("Interrupt Fired");                     // Print message to serial monitor
//  Serial.println("Just woke up!");                       // Next line of code executed after the interrupt
//  digitalWrite(Status_LED, HIGH);                        // Turning LED on
//  sleep_disable();                                       // Disable sleep mode
//  detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN)); // Remove the interrupt from pin 19;
//}