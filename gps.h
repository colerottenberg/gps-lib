#pragma once
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
// #define LAT_DEG
// #define LON_DEG 111111.1
//
/* The GPS recieve multiple message from a range of different satelites
 * The GLL is the most basic format with latitude, longitude, and UTC
 * The GGA has all the of same data with the addition of an altitude
 * Other messages contain more technical data for error correcting
 */
// This can be adjusted based on correct port map to GPS module
// static const char port[] = "/dev/ttyACM0";
// Grouping of data is passed to the struct
// No functions in struct
struct GLL {
  std::string rawData;
  float lat, lon = 0.0;
  double utc = 0.0;
};
struct GGA {
  std::string rawData;
  float lat, lon, msl = 0.0;
  double utc = 0.0;
};
class GPS {
private:
  GLL gll;
  GGA gga;

public:
  static int open_serial_connection();
  static std::string searchGLL(char *buff);
  static std::string searchGGA(char *buff);
  GPS();
  ~GPS();
  bool isGLLEmpty();
  bool isGGAEmpty();
  void initGLL(std::string raw);
  void initGGA(std::string raw);
  GLL getGLL() { return this->gll; };
  GGA getGGA() { return this->gga; };
  std::string getCoords();
  std::string log();
  static bool recv_data(int fd, GPS *c);
};
