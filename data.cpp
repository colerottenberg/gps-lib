#include <cerrno>
#include <chrono>
#include <cmath>
#include <fcntl.h>
#include <fstream>
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

#include "gps.h"

using namespace std;
int main(int argc, char *argv[]) {
  // Open the serial port
  int t = 10;
  string port = "/dev/ttyACM0";
  if (argc == 1) {
    // nothing happens here
  } else if (argc == 2) {
    // set the first arg to t as in the number of times we parse gps data
    t = std::stoi(argv[1]);
  } else if (argc == 3) {
    // set the first arg to t as in the number of times we parse gps data
    // set arg 2 to the correct serial port
    t = std::stoi(argv[1]);
    port = argv[2];
  } else {
    // print too many args
    std::cerr << "Too many arguments!" << endl;
    return 1;
  }
  fstream fout;
  fout.open("log.csv", ios::out | ios::app);
  int fd = GPS::open_serial_connection(port);
  if (fd == -1) {
    std::cerr << "Failed to open serial port." << std::endl;
    return 1;
  } else {
    // read serial port
    int i = 1;
    GPS c;
    while (i <= t) {
      bool valid = GPS::recv_data(fd, &c);
      if (valid) {
        // now we have data in this GPS class which we will need to delete
        // we can now impl the logic about how to write to a file
        std::string log = c.log();
        if (!log.empty()) {
          // sleep(6);
          cout << i << ',' << log;
          fout << log;
          // now for percent finished
          float per = ((float)i / (float)t) * 100.0;
          cout << std::to_string((int)round(per)) << "%" << endl;
          i++;
        }
      }
    }
    fout.close();
  }
};
// Write to the serial port
// const char *writeData = "Hello, device!";
// int bytesWritten = write(serialPort, writeData, strlen(writeData));
// if (bytesWritten == -1) {
// std
