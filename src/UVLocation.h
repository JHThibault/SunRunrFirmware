#ifndef UV_LOCATION_H
#define UV_LOCATION_H



//-------------------------------------------------------------------

class UVLocation {

private:
    int mills;
    int hour;
    int minute;
    int second;
    float longitude;
    float latitude;
    float speed;
    float uvValue;
    char* state;



public:
    UVLocation();
    UVLocation(int mills, int hour, int minute, int second, float longitude, float latitude, float speed, float uvValue, char* state);
    float getLongitude();
    float getLatitude();
    float getUV();
    float getSpeed();
    int getMills();
    int getHour();
    int getMinute();
    int getSecond();
    char* getState();

};

//-------------------------------------------------------------------

#endif
