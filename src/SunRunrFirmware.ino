//-------------------------------------------------------------------

#include "Reporter.h"
// #include "PotholeDetector.h"
#include "UVLocation.h"
#include <AssetTracker.h>
#include <Adafruit_VEML6070.h>
#include <queue>

//-------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

#define button  D5

//-------------------------------------------------------------------

bool executeStateMachines = false;
bool activity = false;
volatile bool buttonDB = false;

//-------------------------------------------------------------------

AssetTracker locationTracker = AssetTracker();  //make GPS Object
Adafruit_VEML6070 UVTracker = Adafruit_VEML6070(); //UVTracker Object
queue<UVLocation> locationsQueue;        //queue of Locations
// PotholeDetector potholeDetector(locationTracker, potholeLocations, 2, 10, 10000.0);
Reporter Reporter(locationTracker, locationsQueue);

UVLocation locData;
String postData;
String timeNow;

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

    pinMode(D7, OUTPUT);
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


    locationTracker.updateGPS();  //get the curent gps location

    if (buttonDB == true) {
      activity = !activity;
      delay(50); //wait for transitnts to die out for the button
      buttonDB = false;
    }

    if(activity == false){
      digitalWrite(D7, LOW);
      //Serial.println("waiting to start");

    }
    else if (executeStateMachines) {   //when the intrupt changes this flag to true
      digitalWrite(D7, HIGH);
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
        executeStateMachines = false;


    }

    //outsize the loop excuite the reporter
    Reporter.execute();
}

//-------------------------------------------------------------------
