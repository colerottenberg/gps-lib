#include "gps.h"
#include <string>

gps::gps() { return; };
void gps::init(std::string raw) {
  // Raw data format: 3901.20328,N,07725.64271,W
  // so we need a to use substr as this is const
  float latDeg = (float)std::stoi(raw.substr(0, 2));
  float lonDeg = (float)std::stoi(raw.substr(14, 2));
  // Need to divide the minutes by 60... thanks brendan
  float latMin = std::stof(raw.substr(2, 8));
  float lonMin = std::stof(raw.substr(16, 8));
  latMin /= 60;
  lonMin /= 60;
  latDeg += latMin;
  lonDeg += lonMin;
  this->lat = latDeg;
  this->lon = lonDeg;
};
std::string gps::getCoords() {
  std::string lat, lon, c;
  lat = std::to_string(this->lat);
  lon = std::to_string(this->lon);
  c.append(lat);
  c.append("°N, -");
  c.append(lon);
  c.append("°W");
  return c;
}
