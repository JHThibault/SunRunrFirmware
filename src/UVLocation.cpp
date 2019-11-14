//-------------------------------------------------------------------

#include "UVLocation.h"

//-------------------------------------------------------------------

UVLocation::UVLocation() {
   this->longitude = 0.0;
   this->latitude = 0.0;
   this->uvValue = 0.0;
   this->speed = 0.0;
   this->mills = 0;
}

//-------------------------------------------------------------------

UVLocation::UVLocation(int mills, float longitude, float latitude, float speed, float uvValue) {
   this->mills = mills;
   this->longitude = longitude;
   this->latitude = latitude;
   this->speed = speed;
   this->uvValue = uvValue;
}

//-------------------------------------------------------------------

float UVLocation::getLongitude() {
   return longitude;
}

//-------------------------------------------------------------------

float UVLocation::getLatitude() {
   return latitude;
}

//-------------------------------------------------------------------

float UVLocation::getUV() {
   return uvValue;
}

//-------------------------------------------------------------------

float UVLocation::getSpeed() {
   return speed;

}

//-------------------------------------------------------------------

int UVLocation::getMills() {
   return mills;

}

//-------------------------------------------------------------------
