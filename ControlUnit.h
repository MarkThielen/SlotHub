#pragma once

#include <string>
#include <thread>

class ControlUnit {




 private:
  
  std::string tty;
  
  int fileDescriptor;

  int set_interface_attribs (int fd, int speed, int parity);
  void set_blocking (int fd, int should_block);


 public:

  ControlUnit(std::string ttyToUse);

  int getFileDescriptor();
  

  ~ControlUnit();

  void run();

  std::thread start();


};
