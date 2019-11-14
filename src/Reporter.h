#ifndef POTHOLE_REPORTER_H
#define POTHOLE_REPORTER_H

//-------------------------------------------------------------------

#include <AssetTracker.h>
#include "UVLocation.h"
#include <queue>

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
    int led;
    AssetTracker& gpsSensor;
    queue<UVLocation>& locationsQueue;
public:
    Reporter(AssetTracker &theTracker, queue<UVLocation>& theLocations);
    void execute();
};

//-------------------------------------------------------------------

#endif
