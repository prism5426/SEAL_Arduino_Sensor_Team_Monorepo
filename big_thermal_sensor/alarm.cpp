#include "alarm.h"

const uint16_t t1_load = 0;

// check distance and other parameters to set alarmStatus flag and blinkRate
void updateAlarm(bool* alarmStatus, uint16_t* blinkRate, bool* thermalCam, float* pixels[AMG_COLS * AMG_ROWS], float* distance, TIMER_STATE* state) {
//      Serial.print("thermalCam status = ");
//      Serial.println(*thermalCam);
      // check if thermalCam is on
      if (*thermalCam) {
          // turn on timer if not 
          if (*state == TIMER_STATE_HALT) {
              *state = TIMER_STATE_START;
              start_timer();  
          }  
          double max_value = (*pixels)[0];       
          // check for person
          *alarmStatus = 0;
          for (int i = 0; i < 64; i++) {
              if ((*pixels)[i] > max_value) {
                max_value = (*pixels)[i];
              }
          }
          // if the maximum of 64 pixels is greater than maximum threshold (target above ambient temp), flash LED
          if (max_value >= MaxTemp) {
                *alarmStatus = 1;

                // if human is less than 3 feet away, blink rapidly
                if ((RoundUp((uint16_t)(*distance)) / 304.8) < 3) *blinkRate = 80000;
                // if human is more than 3 feet away, blink slowly
                else *blinkRate = 30000;
                
                if (OCR1A == 0 || OCR1A - 1000 > *blinkRate || OCR1A + 1000 < *blinkRate) {
                    OCR1A = *blinkRate; // person detected, update blinkRate based on distance
                }
                return;
            }
          // if person not detected
          if (!(*alarmStatus)) {
              stop_timer();
              *state = TIMER_STATE_HALT;
          }
      } else {
          // turn off led and timer
          stop_timer();
          *state = TIMER_STATE_HALT;
      }   
}


void alarmTask(void* aData) {
    alarmData* data = (alarmData*)aData;
    updateAlarm(data->alarmStatus, data->blinkRate, data->tofData->thermalCam, data->pixels, data->tofData->distance, data->state);  
}

// initialize timer
void timer_init() {
    DDRA |= _BV(ALARM_LED); // set ALARM_LED as output mode

    // Reset timer1 control reg A
    TCCR1A = 0;

    // set CTC mode
    TCCR1B &= ~_BV(WGM13);
    TCCR1B |= _BV(WGM12);
    
    // set prescaler of 256: 16mhz / 256
    TCCR1B |=  _BV(CS12); 
    TCCR1B &= ~_BV(CS11);
    TCCR1B &= ~_BV(CS10);

    // reset timer count to 0
    TCNT1 = t1_load;

    // disable timer1 compare interrupt
    TIMSK1 &= ~_BV(OCIE1A);

    // enable gloabal interrupt
    sei();
}

// start timer
void start_timer() {
    // set prescaler of 256: 16mhz / 256
    TCCR1B |=  _BV(CS12); 
    TCCR1B &= ~_BV(CS11);
    TCCR1B &= ~_BV(CS10);
    
    // enable timer1 compare interrupt
    TIMSK1 |= _BV(OCIE1A);
}

// stop timer
void stop_timer() {
    // disable timer1 compare interrupt
    TIMSK1 &= ~_BV(OCIE1A);

    // disable timer
    TCCR1B &= ~_BV(CS12); 
    TCCR1B &= ~_BV(CS11);
    TCCR1B &= ~_BV(CS10);
    
    // turn off led
    PORTA &= ~_BV(ALARM_LED); 
}

ISR(TIMER1_COMPA_vect) {
    // toggle led
    PORTA ^= _BV(ALARM_LED); 
}

/*ISR(TIMER1_OVF_vect) {
    Serial.println("led on");
    // turn on led
    PORTA |= _BV(ALARM_LED);  
}*/

uint16_t RoundUp(uint16_t toRound)
{
     if (toRound % 10 == 0) return toRound;
     return (10 - toRound % 10) + toRound;
}
