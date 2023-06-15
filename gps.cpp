#include "gps.h"
#include <string>

GPS::GPS() { return; };
void GPS::initGLL(std::string raw) {
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
  this->gll.lat = latDeg;
  this->gll.lon = lonDeg;
};
void GPS::initGGA(std::string raw) {
  // Raw data format:
  // 175508.00,3901.22104,N,07725.63373,W,1,12,0.84,85.3,M,-34.5,M,,*4B
  // so we need a to use substr as this is const
  double utc = (double)std::stod(raw.substr(0, 9));
  float latDeg = (float)std::stoi(raw.substr(10, 2));
  float lonDeg = (float)std::stoi(raw.substr(24, 2));
  // Need to divide the minutes by 60... thanks brendan
  float latMin = std::stof(raw.substr(12, 8));
  float lonMin = std::stof(raw.substr(26, 8));
  float msl = std::stof(raw.substr(47, 3));
  latMin /= 60;
  lonMin /= 60;
  latDeg += latMin;
  lonDeg += lonMin;
  this->gga.lat = latDeg;
  this->gga.lon = lonDeg;
  this->gga.msl = msl;
  this->gga.utc = utc;
};
bool GPS::isGGAEmpty() {
  if (gga.lat != 0.0 && gga.lon != 0.0 && gga.msl != 0.0) {
    return false;
  }
  return true;
}
bool GPS::isGLLEmpty() {
  if (this->gll.lat != 0.0 && gll.lon != 0.0) {
    return false;
  }
  return true;
}
std::string GPS::getCoords() {
  std::string utc, lat, lon, msl, c;
  if (isGGAEmpty()) {
    utc = std::to_string(this->gll.utc);
    lat = std::to_string(this->gll.lat);
    lon = std::to_string(this->gll.lon);
    c.append("UTC: ");
    c.append(utc);
    c.append(" ");
    c.append(lat);
    c.append("°N, -");
    c.append(lon);
    c.append("°W");
  } else if (isGLLEmpty()) {
    // both are empty return empty string
    return c;
  } else {
    utc = std::to_string(this->gga.utc);
    lat = std::to_string(this->gga.lat);
    lon = std::to_string(this->gga.lon);
    msl = std::to_string(this->gga.msl);
    c.append("UTC: ");
    c.append(utc);
    c.append(" ");
    c.append(lat);
    c.append("°N, -");
    c.append(lon);
    c.append("°W, ");
    c.append(msl);
    c.append("MSL");
  }
  return c;
}
