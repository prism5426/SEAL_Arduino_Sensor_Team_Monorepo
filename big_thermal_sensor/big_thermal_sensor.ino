#include "big_thermal_sensor.h"

#define PIXEL_DEBUG 1
#define DIST_DEBUG 0

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 18

int buttonState = 0;         // variable for reading the pushbutton status

// tft driver
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Task Control Blocks
TCB thermalSensorTCB;
TCB displayTCB;
TCB touchInputTCB;
TCB alarmTCB;
TCB calibrationTCB;
TCB tofTCB;

TCB *head = NULL;
TCB *taskPtr = NULL;
TCB *tail = NULL;

// display task data
displayTaskData dData;

// display history data
displayHistory dhData;

// thermal sensor task data
Adafruit_AMG88xx amg;
thermalSensorData thData;
float pixels[AMG_COLS * AMG_ROWS];
float HDTemp[HD_ROWS * HD_COLS];
float MIN_TEMP = 25;
bool thermalCam = 0;
bool prev_thermalCam;

// calibration task data
calibrationData cData;

// alarm task data
alarmData aData;
bool alarmStatus;
TIMER_STATE state = TIMER_STATE_HALT;
uint16_t blinkRate = 0;

// tof task data
tofSensorData tofData;
float distance;
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void setup(void)
{
  // Initialize ToF
  pinMode(Status_LED, OUTPUT);          // Use the led on digital pin 37 to indicate status of Arduino
  pinMode(buttonPin, INPUT);           // Use Button on digital pin 41
  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Set digital pin 19 to input using the buildin pullup resistor
  //    digitalWrite(Status_LED, HIGH);       // Turn LED on
  Wire.begin();

  //   Serial.println("VL53L1X Qwiic Test");
  if (distanceSensor.begin() != 0) // Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  //    Serial.println("Sensor online!");
  // 8ft is 2438.4 mm -> rounded to 2500mm
  distanceSensor.setInterruptPolarityLow();
  distanceSensor.setDistanceModeLong();
  //    distanceSensor.setDistanceThreshold(10, 2500, 3);
  distanceSensor.startRanging(); // Write configuration bytes to initiate measurement

  // Initialize serial communication
  Serial.begin(9600);

  // setup calibration
  //attachInterrupt(digitalPinToInterrupt(buttonPin),ISR, RISING);

  // Initialize Display and displayHistory
  tft.reset();
  pinMode(XM, OUTPUT);
  pinMode(YM, OUTPUT);
  dhData = {&prev_thermalCam};
  dData = {&tft, &dhData, &thData, &MIN_TEMP};
  displayTCB.task = &displayTask;
  displayTCB.taskDataPtr = &dData;
  displayTCB.next = NULL;
  displayTCB.prev = NULL;

  // Initialize thermal sensor
  thermal_sensor_setup();
  thData = {&thermalCam, &amg, pixels, HDTemp};
  thermalSensorTCB.task = &thermalSensorTask;
  thermalSensorTCB.taskDataPtr = &thData;
  thermalSensorTCB.next = NULL;
  thermalSensorTCB.prev = NULL;

  // Initialize ToF
  tofData = {&distanceSensor, &distance, &thermalCam};
  tofTCB.task = &tofSensorTask;
  tofTCB.taskDataPtr = &tofData;
  tofTCB.next = NULL;
  tofTCB.prev = NULL;

  // Initialize calibration
  cData = {&distance, pixels, &MIN_TEMP};
  calibrationTCB.task = &alarmTask;
  calibrationTCB.taskDataPtr = &cData;
  calibrationTCB.next = NULL;
  calibrationTCB.prev = NULL;

  // Initialize alarm led
  timer_init();
  aData = {&tofData, &alarmStatus, &state, &blinkRate, pixels};
  alarmTCB.task = &alarmTask;
  alarmTCB.taskDataPtr = &aData;
  alarmTCB.next = NULL;
  alarmTCB.prev = NULL;

  // initialize tasks
  insertTask(&displayTCB);
  insertTask(&thermalSensorTCB);
  insertTask(&alarmTCB);
  insertTask(&tofTCB);
  insertTask(&calibrationTCB);
    
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325)
  {
    //        Serial.println(F("Found ILI9325 LCD driver"));
  }
  else if (identifier == 0x9328)
  {
    Serial.println(F("Found ILI9328 LCD driver"));
  }
  else if (identifier == 0x4535)
  {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }
  else if (identifier == 0x7575)
  {
    Serial.println(F("Found HX8347G LCD driver"));
  }
  else if (identifier == 0x9341)
  {
    //        Serial.println(F("Found ILI9341 LCD driver"));
  }
  else if (identifier == 0x8357)
  {
    Serial.println(F("Found HX8357D LCD driver"));
  }
  else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    //        Serial.println(F("Found 0x9341 LCD driver"));
  }
  else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, float-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;
  }

  tft.begin(identifier);

  // initialize black screen
  tft.fillScreen(BLACK);
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
    digitalWrite(Status_LED, LOW); // Turn LED on
    //        Going_To_Sleep();
  }
}

int counter;
float total_min_value;

void loop(void)
{ 
  /* debugging
  if (PIXEL_DEBUG) print_pixels();
  if (DIST_DEBUG) print_distance(); 
  */
  
  float distance = (distanceSensor.getDistance()) / 304.8;
  tofSensorRoutine(distance);

  double min_value = pixels[0];
  for (int i = 0; i < 64; i++) {
    if (pixels[i] < min_value) {
      min_value = pixels[i];
    }
  }
  if (distance > 0.5) {
    counter++;
    total_min_value += min_value;
  }

  if (counter == 10) {
    counter = 0;
    MIN_TEMP = total_min_value / 10;
    total_min_value = 0;
  }
//    Serial.print(MIN_TEMP);
//    Serial.print(",");
//    Serial.print(min_value);
//    Serial.print(",");
//    Serial.print(distance);
//    Serial.println();
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

void Going_To_Sleep()
{
  Serial.println(F("Entering sleep mode!"));
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), wakeUp, FALLING);
  digitalWrite(Status_LED, LOW); // Turn LED off
  delay(1000);                  // Wait a second to allow the led to be turned off before going to sleep
  Serial.flush();
  interrupts();
  sleep_cpu();
}

void wakeUp()
{
  Serial.println();
  Serial.println("Interrupt Fired");                     // Print message to serial monitor
  Serial.println("Just woke up!");                       // Next line of code executed after the interrupt
  digitalWrite(Status_LED, HIGH);                        // Turning LED on
  sleep_disable();                                       // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN)); // Remove the interrupt from pin 19;
}
