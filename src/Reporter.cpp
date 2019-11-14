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
}

//-------------------------------------------------------------------

void Reporter::execute() {
    UVLocation nextLocation;
    String postData;

    Serial.println(locationsQueue.size());

    switch (state) {
        case Reporter::S_Wait:
            tick = 0;
            digitalWrite(led, LOW);  //turn off the LED

            if (locationsQueue.size() > 0) {  //if there is anything to report
                state = Reporter::S_Publish;
            }
            else { //else just stay here
                state = Reporter::S_Wait;
            }
            break;

        case Reporter::S_Publish:

            for (int i = 0; i < locationsQueue.size(); i++) {
              nextLocation = locationsQueue.front();  //get the front locatoin form the queue
              locationsQueue.pop();                   //removes the front element

              postData += String::format("{ \"Time\": \"%d\",\"longitude\": \"%f\", \"latitude\": \"%f\" , \"speed\": \"%f\", \"uv\": \"%f\"} \n",
                                         nextLocation.getMills(), nextLocation.getLongitude(), nextLocation.getLatitude(),
                                         nextLocation.getSpeed(), nextLocation.getUV());
            }



            //**********************************************************************************************

            Serial.println(postData);  //print the datastring
            postData = ""; //wipe the buffer
            Particle.publish("sunRun", postData);  //post the data.
            state = Reporter::S_LedNotify; //set the next state
            break;

        case Reporter::S_LedNotify:
            digitalWrite(led, HIGH);  //turn on the LED
            ++tick;

            // Keep LED on for 1 second
            if (tick == 100) { //after 100 ticks go back to the wait state
                state = Reporter::S_Wait;
            }
            else {
                state = Reporter::S_LedNotify; //stay here till then
            }
            break;
    }
}

//-------------------------------------------------------------------
