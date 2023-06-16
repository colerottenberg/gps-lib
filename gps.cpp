#include "gps.h"
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
#include <unistd.h>
using std::string;
using namespace std;
GPS::GPS() { return; };
GPS::~GPS() { return; };
void GPS::initGLL(std::string raw) {
  //$GNGLL,3901.21817,N,07725.63343,W,144737.00,A,A*67
  // Raw data format: 3901.20328,N,07725.64271,W
  // so we need a to use substr as this is const
  float latDeg = (float)std::stoi(raw.substr(0, 2));
  float lonDeg = (float)std::stoi(raw.substr(14, 2));
  // Need to divide the minutes by 60... thanks brendan
  float latMin = std::stof(raw.substr(2, 8));
  float lonMin = std::stof(raw.substr(16, 8));
  double utc = std::stod(raw.substr(27, 9));
  latMin /= 60;
  lonMin /= 60;
  latDeg += latMin;
  lonDeg += lonMin;
  this->gll.lat = latDeg;
  this->gll.lon = lonDeg;
  this->gll.utc = utc;
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
  float msl = std::stof(raw.substr(47, 4));
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
std::string GPS::log() {
  std::string utc, lat, lon, msl, c;
  if (isGGAEmpty()) {
    utc = std::to_string(this->gll.utc);
    lat = std::to_string(this->gll.lat);
    lon = std::to_string(this->gll.lon);
    c.append(utc);
    c.append(",");
    c.append(lat);
    c.append(",-");
    c.append(lon);
    c += '\n';
  } else if (isGLLEmpty()) {
    // both are empty return empty string
    return c;
  } else {
    utc = std::to_string(this->gga.utc);
    lat = std::to_string(this->gga.lat);
    lon = std::to_string(this->gga.lon);
    msl = std::to_string(this->gga.msl);
    c.append(utc);
    c.append(",");
    c.append(lat);
    c.append(",-");
    c.append(lon);
    c.append(",");
    c.append(msl);
    c += '\n';
  }
  return c;
}
int GPS::open_serial_connection() {
  // https://stackoverflow.com/questions/22544751/how-to-send-and-read-data-from-serial-port-in-c
  int fd;
  const char port[] = "/dev/ttyACM0";
  printf("Starting\n");
  fd = open(port, O_RDWR | O_NONBLOCK);
  if (fd == -1) {
    printf("Failed to open serial connection\n");
    exit(1);
  }
  printf("Serial connection open\n");
  struct termios tios;
  tcgetattr(fd, &tios);
  tios.c_iflag = IGNBRK | IGNPAR;
  tios.c_oflag = 0;
  tios.c_lflag = 0;
  cfsetspeed(&tios, B9600);
  if (tcsetattr(fd, TCSAFLUSH, &tios) == -1) {
    printf("tcsetattr failed");
    exit(2);
  }
  usleep(1000);
  printf("Serial connection configured\n");
  return fd;
};
std::string GPS::searchGLL(char *buff) {
  string coords;
  string buffAsStr = buff;
  string geo1 = "$GNGLL";
  string geo2 = "$GNGGA";
  string header = buffAsStr.substr(0, 6);
  if (header == geo1) {
    // found
    coords = buffAsStr.substr(7);
  }
  return coords;
};
std::string GPS::searchGGA(char *buff) {
  string coords;
  string buffAsStr = buff;
  string geo1 = "$GNGLL";
  string geo2 = "$GNGGA";
  string header = buffAsStr.substr(0, 6);
  if (header == geo2) {
    // found
    coords = buffAsStr.substr(7);
  }
  return coords;
};
GPS *GPS::recv_data(int fd) {
  // read messages over serial connection the using all the given methods to the
  // library parse and clean the data start loop until read over
  GPS *c = new GPS();
  char buff[512];
  int bytesRead = 0;
  while (bytesRead <= 0) {
    // we read through the fd
    bytesRead = read(fd, &buff, sizeof(buff));
    // cont. loop until buffer is filled
  }
  // cout << string(buff) << endl;
  // now we need to parse the data as before... first as GGA then GLL
  string ggaStr = searchGGA(buff);
  string gllStr = searchGLL(buff);
  if (!ggaStr.empty()) {
    c->initGGA(ggaStr);
  } else if (!gllStr.empty()) {
    c->initGLL(gllStr);
  } else {
    delete c;
    return nullptr;
  }
  return c;
};
