//-------------------------------------------------------------------

#include "Reporter.h"
#include "UVLocation.h"




//-------------------------------------------------------------------

Reporter::Reporter(AssetTracker &theTracker,
                                 queue<UVLocation> &theLocations) : gpsSensor(theTracker),  //object of GPS
                                                                         locationsQueue(theLocations)  //queue of Locations
                                                                         {

    tick = 0;
    state = S_Wait;
    led = D7;
    pinMode(led, OUTPUT);  //set the LED as an OUTPUT
    digitalWrite(led, LOW);
}

//-------------------------------------------------------------------

void Reporter::execute() {
    UVLocation nextLocation;
    String postData;

    switch (state) {
        case Reporter::S_Wait:
            tick = 0;

            if (locationsQueue.size() > 0) {  //if there is anything to report
                state = Reporter::S_Publish;
            }
            else { //else just stay here
                state = Reporter::S_Wait;
            }
            break;

        case Reporter::S_Publish:

            digitalWrite(led, HIGH);  //turn off the LED

            locData = locationsQueue.front();  //get the front locatoin form the queue
            locationsQueue.pop();                   //removes the front element

            postData = String::format("{ \"Time\": \"%d:%d:%02d\",\"longitude\": \"%f\", \"latitude\": \"%f\" , \"speed\": \"%f\", \"uv\": \"%f\"}",
                                          locData.getHour(), locData.getMinute(), locData.getSecond(), locData.getLongitude(), locData.getLatitude(),
                                          locData.getSpeed(), locData.getUV());

            Serial.println(postData);  //print the datastring
            Particle.publish("sunRun", postData);  //post the data.
            state = Reporter::S_LedNotify; //set the next state
            postData = ""; //wipe the buffer
            break;

        case Reporter::S_LedNotify:
            digitalWrite(led, LOW);  //turn on the LED
            ++tick;

            // Keep LED on for 1 second
            if (tick == 100) { //after 50 ticks go back to the wait state
                state = Reporter::S_Wait;
            }
            else {
                state = Reporter::S_LedNotify; //stay here till then
            }
            break;
    }
}

//-------------------------------------------------------------------
