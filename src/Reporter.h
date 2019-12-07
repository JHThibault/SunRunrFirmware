#ifndef POTHOLE_REPORTER_H
#define POTHOLE_REPORTER_H

//-------------------------------------------------------------------

#include <AssetTracker.h>
#include "UVLocation.h"
#include <queue>

#define statusLED D7
#define uvLED D6
#define ONE_DAY_MILLIS 24 * 60 * 60 * 1000
#define button  D5

//-------------------------------------------------------------------

using namespace std;



//-------------------------------------------------------------------

class Reporter {
   enum State { S_Wait, S_Publish, S_LedNotify };

private:
    int rate;

private:
    State state;
    int tick;
    AssetTracker& gpsSensor;
    queue<UVLocation>& locationsQueue;
    UVLocation locData;
public:
    Reporter(AssetTracker &theTracker, queue<UVLocation>& theLocations);
    void execute();
};

//-------------------------------------------------------------------

#endif
