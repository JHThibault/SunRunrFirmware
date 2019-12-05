//-------------------------------------------------------------------

#include "Reporter.h"
#include "UVLocation.h"



//-------------------------------------------------------------------

Reporter::Reporter(AssetTracker &theTracker,
                                 queue<UVLocation> &theLocations, leds &ledsP) : gpsSensor(theTracker),  //object of GPS
                                                                         locationsQueue(theLocations),  //queue of Locations
                                                                         ledsR(ledsP){
    tick = 0;
    state = S_Wait;
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

            ledsR.reportBlink();

            state = Reporter::S_LedNotify;
            // ++tick;
            //
            // // this dose not work the leds any more bit it will give a delay so we dont stack reports to fast
            // if (tick == 100) { //after 50 ticks go back to the wait state
            //     state = Reporter::S_Wait;
            // }
            // else {
            //     state = Reporter::S_LedNotify; //stay here till then
            // }
            break;
    }
}

//-------------------------------------------------------------------
