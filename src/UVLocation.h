#ifndef UV_LOCATION_H
#define UV_LOCATION_H

//-------------------------------------------------------------------

class UVLocation {

private:
    int mills;
    float longitude;
    float latitude;
    float speed;
    float uvValue;


public:
    UVLocation();
    UVLocation(int mills, float longitude, float latitude, float speed, float uvValue);
    float getLongitude();
    float getLatitude();
    float getUV();
    float getSpeed();
    int getMills();

};

//-------------------------------------------------------------------

#endif
