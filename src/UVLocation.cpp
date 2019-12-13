//-------------------------------------------------------------------

#include "UVLocation.h"

//-------------------------------------------------------------------

UVLocation::UVLocation() {
   this->longitude = 0.0;
   this->latitude = 0.0;
   this->uvValue = 0.0;
   this->speed = 0.0;
   this->mills = 0;
   this->hour = 0;
   this->minute = 0;
   this->second = 0;
   this->state = "null";

}

//-------------------------------------------------------------------

UVLocation::UVLocation(int mills, int hour, int minute, int second, float longitude, float latitude, float speed, float uvValue, char* state) {
   this->mills = mills;
   this->hour = hour;
   this->minute = minute;
   this->second = second;
   this->longitude = longitude;
   this->latitude = latitude;
   this->speed = speed;
   this->uvValue = uvValue;
   this->state = state;
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

int UVLocation::getHour() {
   return hour;

}

//-------------------------------------------------------------------

int UVLocation::getMinute() {
   return minute;

}

//-------------------------------------------------------------------

int UVLocation::getSecond() {
   return second;

}

char* UVLocation::getState(){
  return state;
}

//-------------------------------------------------------------------
