//-------------------------------------------------------------------

#include "Reporter.h"
// #include "leds.h"
// #include "PotholeDetector.h"
#include "UVLocation.h"
#include <AssetTracker.h>
#include <Adafruit_VEML6070.h>
#include <queue>


SYSTEM_THREAD(ENABLED);




//-------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------
#define delayTime  5000


unsigned long lastSync = millis();




//-------------------------------------------------------------------

volatile bool executeStateMachines = false;
bool activity = false;
// bool moving = false;


//-------------------------------------------------------------------

AssetTracker locationTracker = AssetTracker();  //make GPS Object
Adafruit_VEML6070 UVTracker = Adafruit_VEML6070(); //UVTracker Object
queue<UVLocation> locationsQueue;        //queue of Locations
// PotholeDetector potholeDetector(locationTracker, potholeLocations, 2, 10, 10000.0);
Reporter Reporter(locationTracker, locationsQueue);

enum State { S_Wait, S_Paused, S_deBounce, S_Activity };
volatile State state;
volatile State lastState;

UVLocation locData;
UVLocation lastLocation;

int uvThreshold = 1000;

int waitLedTick = 0;
int pauseCount = 0;
float uvValue;

//-------------------------------------------------------------------

void stateMachineScheduler() {        //sets bullen to exuicute the state machine
    executeStateMachines = true;
}

Timer stateMachineTimer(delayTime, stateMachineScheduler);   //when 5 secs have passed call functuoin

//-------------------------------------------------------------------

void responseHandler(const char *event, const char *data) {     // prints the data that comes back form the server
    // Formatting output
    String output = String::format("POST Response:\n  %s\n  %s\n", event, data);
    // Log to Serial console
    Serial.println(output);
    //PARSE Out the UV
    uvThreshold = 1000; /////// Fix me witht the server feedback

}

void buttonHandler(){
    lastState = state;
    state = S_deBounce;
}

//-------------------------------------------------------------------

void setup() {
    Serial.begin(9600);

    // Initialize the gps and turn it on
    locationTracker.begin();
    locationTracker.gpsOn();

    //Initialize the UV sensor
    UVTracker.begin(VEML6070_1_T);

    // Handler for response from POSTing location to server
    Particle.subscribe("hook-response/sunRun", responseHandler, MY_DEVICES);  //set up the link with the webhook

    stateMachineTimer.start();          //activate the timer interupt

    //init the LEDS
    pinMode(statusLED, OUTPUT);
    pinMode(uvLED, OUTPUT);


    pinMode(button, INPUT_PULLUP); //set button as an input
    attachInterrupt(button, buttonHandler, FALLING); //atatch the intrupt

    state = S_Wait;  //first go to the wait state fitst
    waitLedTick = 0;

}

//-------------------------------------------------------------------

void loop() {

    // Request time synchronization from the Particle Cloud once per day
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }

    //Drop the times after 24 hours
    if(locationsQueue.size() > 1000){ //if the stack gets to big dump out the old points
      lastLocation = locationsQueue.front();
      if(lastLocation.getMills() > (millis()-ONE_DAY_MILLIS)){
        locationsQueue.pop();
      }
    }

    locationTracker.updateGPS();  //get the curent gps location
    uvValue = UVTracker.readUV(); //save the curent uv value


    //check the uv warning light
    if(uvValue>uvThreshold){
      digitalWrite(uvLED, HIGH);
    }
    else{
      digitalWrite(uvLED, LOW);
    }


    switch (state) {

      //inital wait state
      case S_Wait:
        Serial.print("wait\t");
        Serial.println(waitLedTick);
          if(waitLedTick<1500){
            digitalWrite(statusLED, LOW);
          }
          if(waitLedTick>1500){
            digitalWrite(statusLED, HIGH);
          }
          if(waitLedTick > 3000){
            waitLedTick = 0;
          }

          waitLedTick++;

          state = S_Wait;

          break;

      //30 senconds without movement wait state.
      case S_Paused:

            Serial.println("paused");
          //led blink code
          if(waitLedTick<800){
            digitalWrite(statusLED, HIGH);
          }
          if(waitLedTick>800){
            digitalWrite(statusLED, LOW);
          }
          if(waitLedTick > 1600){
            waitLedTick = 0;
          }

          waitLedTick++;


          //check to see if the speed over 3 mph to start agin.
          if(locationTracker.getSpeed() >= 3){
            state = S_Activity;
          }
          else{
            state = S_Paused;
          }
          break;

      //handles what happens when the button has been presseed
      case S_deBounce:
          Serial.println("debounce");
          delay(100);  //debouce delay to wait for transents to die out.
          if (lastState == S_Activity || lastState == S_Paused){ //stop the activity
            state = S_Wait;
            waitLedTick = 0;
          }
          else{                         //else start the activity
            state = S_Activity;
            pauseCount = 0;
          }
          break;
      case S_Activity:
          Serial.println("activity");
          if (executeStateMachines) {   //when the intrupt changes this flag to true
              digitalWrite(statusLED, LOW); //turn on the status lgiht to show we are in the activity
              locationTracker.updateGPS();  //get the curent gps location

              if (locationTracker.gpsFix()) {  //if there is curently a fix, report the location from the queue
                locData = UVLocation(millis(), Time.hour(), Time.minute(), Time.second(),locationTracker.readLonDeg(), locationTracker.readLatDeg(),
                                  locationTracker.getSpeed(), uvValue);
              }
              else {                     //if we dont curently have a fix return a BS locatoin
                // locData = UVLocation(millis(), Time.hour(), Time.minute(), Time.second(), -110.948676, 32.232609,
                //                   1, uvValue);
                locData = UVLocation(millis(), Time.hour(), Time.minute(), Time.second(), 999.9999, 333.3333,
                                                    -1, uvValue);
              }

              //write the new locData to the locationsQueue
              locationsQueue.push(locData);

              //32.232609, -110.948676 location of the office

              if(locationTracker.getSpeed() < 3){
                pauseCount ++;
              }
              else{
                pauseCount = 0;
              }


              //after 30 seconds without movement go to the poused state
              if(pauseCount > (30*1000/delayTime)){
                state = S_Paused;
                waitLedTick = 0;
              }


              // Reporter.execute();
              executeStateMachines = false; //the time intrupt will change it back when the time is right.
          }
          else{
            digitalWrite(statusLED, HIGH);
          }
          break;

    }

    //if there is wifi post the data points to the server
    if (Particle.connected()) {
    	Reporter.execute();
    }


}

//-------------------------------------------------------------------
