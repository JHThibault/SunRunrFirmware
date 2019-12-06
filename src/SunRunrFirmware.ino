//-------------------------------------------------------------------

#include "Reporter.h"
// #include "leds.h"
// #include "PotholeDetector.h"
#include "UVLocation.h"
#include <AssetTracker.h>
#include <Adafruit_VEML6070.h>
#include <queue>


// SYSTEM_MODE(SEMI_AUTOMATIC)
// SYSTEM_THREAD(DISABLE)




//-------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

#define button  D5

//-------------------------------------------------------------------

volatile bool executeStateMachines = false;
volatile bool buttonDB = false;
bool activity = false;
// bool moving = false;


//-------------------------------------------------------------------

AssetTracker locationTracker = AssetTracker();  //make GPS Object
Adafruit_VEML6070 UVTracker = Adafruit_VEML6070(); //UVTracker Object
queue<UVLocation> locationsQueue;        //queue of Locations
// PotholeDetector potholeDetector(locationTracker, potholeLocations, 2, 10, 10000.0);
Reporter Reporter(locationTracker, locationsQueue);



UVLocation locData;
UVLocation lastLocation;
String postData;
String timeNow;

int uvThreshold = 1000;

int waitLedTick = 0;

//-------------------------------------------------------------------

void stateMachineScheduler() {        //sets bullen to exuicute the state machine
    executeStateMachines = true;
}

Timer stateMachineTimer(5000, stateMachineScheduler);   //when 5 secs have passed call functuoin

//-------------------------------------------------------------------

void responseHandler(const char *event, const char *data) {     // prints the data that comes back form the server
    // Formatting output
    String output = String::format("POST Response:\n  %s\n  %s\n", event, data);
    // Log to serial console
    Serial.println(output);
    //PARSE Out the UV
    uvThreshold = 1000; /////// Fix me


}

void buttonHandler(){
    buttonDB = true;
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


    //check the uv warning light
    if(UVTracker.readUV()>uvThreshold){
      digitalWrite(uvLED, HIGH);
    }
    else{
      digitalWrite(uvLED, LOW);
    }


    locationTracker.updateGPS();  //get the curent gps location

    //debounce State
    if (buttonDB == true) {
      activity = !activity;
      delay(1); //wait for transitnts to die out for the button
      buttonDB = false;
      waitLedTick = 0;
    }

    //a button press wait state
    if(activity == false){

      //Serial.println("waiting to start");


      if(waitLedTick<75){
        digitalWrite(statusLED, LOW);
      }
      else if(waitLedTick>75 && waitLedTick<100){

        digitalWrite(statusLED, HIGH);
      }
      else{
        waitLedTick = 0;
      }

      waitLedTick++;

    }

  //  add if else statment for the going to slow wait state here


    else if (executeStateMachines) {   //when the intrupt changes this flag to true
        digitalWrite(statusLED, LOW); //turn on the status lgiht to show we are in the activity
        locationTracker.updateGPS();  //get the curent gps location

        if (locationTracker.gpsFix()) {  //if there is curently a fix, report the location from the queue
          locData = UVLocation(millis(), Time.hour(), Time.minute(), Time.second(),locationTracker.readLonDeg(), locationTracker.readLatDeg(),
                            locationTracker.getSpeed(), UVTracker.readUV());
        }
        else {                     //if we dont curently have a fix return a BS locatoin
          locData = UVLocation(millis(), Time.hour(), Time.minute(), Time.second(), -110.948676, 32.232609,
                            1, UVTracker.readUV());
        }

        //write the new locData to the locationsQueue
        locationsQueue.push(locData);

        //32.232609, -110.948676 location of the office

        // Reporter.execute();
        executeStateMachines = false; //the time intrupt will change it back when the time is right.


    }
    else{
      digitalWrite(statusLED, HIGH); //turn on the status lgiht to show we are in the activity
    }

    //outsize the loop excuite the reporter
    Reporter.execute();
}

//-------------------------------------------------------------------
