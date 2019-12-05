

#include "leds.h"


// when done remove led form reporter


leds::leds(){
  pinMode(statusLED, OUTPUT);
  pinMode(uvLED, OUTPUT);

  state = S_Wait;
  tick = 0;

}

void leds::ledLoop(){

  switch (state) {
      case leds::S_Wait:
        tick = 0;
        state = S_Wait;
        analogWrite(statusLED, 25);
        digitalWrite(uvLED, LOW);

        break;
      case leds::S_Activity:
        digitalWrite(statusLED, HIGH);
        state = S_Activity;
        break;

      case leds::S_Reprot:

        digitalWrite(statusLED, LOW);  //turn on the LED
        ++tick;

        // Keep LED on for 1 second
        if (tick == 100) { //after 50 ticks go back to the wait state
            state = leds::S_Activity;
        }
        else {
            state = leds::S_Reprot; //stay here till then
        }
        break;

    }

}

void leds::reportBlink(){
  state = S_Reprot;
}

void leds::activityRunning(){
  state = S_Activity;
}

void leds::activityWait(){
  state = S_Wait;
}

void leds::setUvThreshold(int uvThreshold){
  this->uvThreshold = uvThreshold;
}

int leds::getUvThreshold(){
  return uvThreshold;
}

void leds::uvLedOn(){
  digitalWrite(uvLED, HIGH);
}

void leds::uvLedOff(){
  digitalWrite(uvLED, LOW);
}
