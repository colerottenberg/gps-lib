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
  // GPS c;
  fstream fout;
  fout.open("log.csv", ios::out | ios::app);
  int fd = GPS::open_serial_connection();
  if (fd == -1) {
    std::cerr << "Failed to open serial port." << std::endl;
    return 1;
  } else {
    // read serial port
    int i = 1;
    int t = std::stoi(argv[1]);
    while (i <= t) {
      GPS *c = GPS::recv_data(fd);
      if (c) {
        // now we have data in this GPS class which we will need to delete
        // we can now impl the logic about how to write to a file
        std::string log = c->log();
        if (!log.empty()) {
          cout << i << ',' << log;
          fout << log;
          // now for percent finished
          float per = ((float)i / (float)t) * 100.0;
          cout << std::to_string((int)round(per)) << "%" << endl;
          i++;
        }
        delete c;
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
