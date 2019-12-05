#ifndef LEDS_H
#define LEDS_H

using namespace std;
#include "Particle.h"

#define statusLED D7

#define uvLED D6


class leds{

  enum State { S_Wait, S_Activity, S_Reprot };

  private:
    int uvThreshold;
    State state;
    int tick;
  public:
    leds();
    void reportBlink();
    void ledLoop();
    void activityRunning();
    void activityWait();
    void setUvThreshold(int uvThreshold);
    int  getUvThreshold();
    void uvLedOn();
    void uvLedOff();


};


#endif
