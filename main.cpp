#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "gps.h"

using namespace std;
const char port[] = "/dev/ttyACM0";
static int fd;

int open_serial_connection() {
  // https://stackoverflow.com/questions/22544751/how-to-send-and-read-data-from-serial-port-in-c
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
std::string searchGLL(char *buff) {
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
std::string searchGGA(char *buff) {
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
int main() {
  // Open the serial port
  GPS c;
  fd = open_serial_connection();
  if (fd == -1) {
    std::cerr << "Failed to open serial port." << std::endl;
    return 1;
  }

  // read serial port
  while (true) {
    //  buffer for the reading of data
    char readBuffer[512];
    int bytesRead = read(fd, &readBuffer, sizeof(readBuffer));
    // check that the buffer is filled
    if (bytesRead > 0) {
      // std::cout << "Received data: " << readBuffer << std::endl;
      // no longer needed for debugging
      // Process the received data
      // We need to look for the string $GNGLL first!
      // other indicators can be checked later because of there different output
      // Come back to me and determine which GEO works best
      string parsedString = searchGLL(readBuffer);
      string ggaStr = searchGGA(readBuffer);
      // cout << "Raw Coordinates: " << parsedString << endl;
      // pass in the raw data to the init function
      if (!ggaStr.empty()) {
        c.initGGA(ggaStr);
        cout << c.getGGA().utc << endl;
        // c.initGGA(parsedString);
        cout << "Coordinates: " << c.getCoords() << endl;
      } else if (!parsedString.empty()) {
        c.initGLL(parsedString);
        // c.initGGA(parsedString);
        cout << "Coordinates: " << c.getCoords() << endl;
      }
    }
  }
};
// Write to the serial port
// const char *writeData = "Hello, device!";
// int bytesWritten = write(serialPort, writeData, strlen(writeData));
// if (bytesWritten == -1) {
// std
