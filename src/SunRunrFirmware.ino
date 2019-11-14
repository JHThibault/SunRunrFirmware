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

//-------------------------------------------------------------------

bool executeStateMachines = false;

//-------------------------------------------------------------------

AssetTracker locationTracker = AssetTracker();  //make GPS Object
Adafruit_VEML6070 UVTracker = Adafruit_VEML6070(); //UVTracker Object
queue<UVLocation> locationsQueue;        //queue of Locations
// PotholeDetector potholeDetector(locationTracker, potholeLocations, 2, 10, 10000.0);
Reporter Reporter(locationTracker, locationsQueue);

UVLocation test;
String postData;

//-------------------------------------------------------------------

void stateMachineScheduler() {        //sets bullen to exuicute the state machine
    executeStateMachines = true;
}

Timer stateMachineTimer(1000, stateMachineScheduler);   //when 10 mills have passed call functuoin

//-------------------------------------------------------------------

void responseHandler(const char *event, const char *data) {     // prints the data that comes back form the server
    // Formatting output
    String output = String::format("POST Response:\n  %s\n  %s\n", event, data);
    // Log to serial console
    Serial.println(output);
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
}

//-------------------------------------------------------------------

void loop() {

    // Request time synchronization from the Particle Cloud once per day
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }


    if (executeStateMachines) {   //when the intrupt changes this flag to true
        locationTracker.updateGPS();  //get the curent gps location
        // potholeDetector.execute();    //sample for potholes

        //add new UVLocation to the queue
        //locationsQueue.push(UVLocation(millis(), locationTracker.readLonDeg(), locationTracker.readLatDeg(),
        //                    locationTracker.getSpeed(), UVTracker.readUV()));


        //locationsQueue.push(UVLocation(millis(), 5, 4, 3, 2));


        locationTracker.updateGPS();
        //UVLocation test = UVLocation(millis(), 5.0, 4.0, 3.0, 2.0);
        locationsQueue.push(UVLocation(millis(), locationTracker.readLonDeg(), locationTracker.readLatDeg(),
                           locationTracker.getSpeed(), UVTracker.readUV()));

        // test = locationsQueue.front();  //get the front locatoin form the queue
        // locationsQueue.pop();
        // postData = String::format("{ \"Time\": \"%d\",\"longitude\": \"%f\", \"latitude\": \"%f\" , \"speed\": \"%f\", \"uv\": \"%f\"}",
        //                           test.getMills(), test.getLongitude(), test.getLatitude(),
        //                           test.getSpeed(), test.getUV());
        // Serial.println(postData);

        test = UVLocation(millis(), locationTracker.readLonDeg(), locationTracker.readLatDeg(),
                           locationTracker.getSpeed(), UVTracker.readUV());

        postData = String::format("{ \"Time\": \"%d\",\"longitude\": \"%f\", \"latitude\": \"%f\" , \"speed\": \"%f\", \"uv\": \"%f\"}",
                                                      test.getMills(), test.getLongitude(), test.getLatitude(),
                                                      test.getSpeed(), test.getUV());
        Serial.println(postData);
        Particle.publish("sunRun", postData);

        //Reporter.execute();
        executeStateMachines = false;


    }
}

//-------------------------------------------------------------------
