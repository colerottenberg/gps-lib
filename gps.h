#pragma once
#include <string>
// #define LAT_DEG
// #define LON_DEG 111111.1
//
class gps {
private:
  float lat, lon = 0.0;
  // static const char port[] = "/dev/ttyACM0";

public:
  gps();
  void init(std::string raw);
  float getLat() { return lat; };
  float getLon() { return lon; };
  std::string getCoords();
};
