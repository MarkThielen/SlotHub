#pragma once

#include <string>
#include <thread>

#include <zmq.hpp>

#include "CarStatus.h"
#include "TrackStatus.h"


class ControlUnit {




 private:
  
  std::string tty;

  std::string bindAddress;
  
  int fileDescriptor;

  zmq::context_t *context;
  zmq::socket_t *publisher;
  
  bool stop_execution;

  int set_interface_attribs (int fd, int speed, int parity);
  void set_blocking (int fd, int should_block);

  void initMessageQueue(std::string bindAddressToUse);

  void sendCarStatusMessage(SlotHub::CarStatusMessage *csm);
  void sendTrackStatusMessage( SlotHub::TrackStatusMessage *tsm  );


 public:

  ControlUnit(std::string ttyToUse, std::string bindAddressToUse);

  int getFileDescriptor();

  std::string getBindAddress();
  void setBindAddress(std::string bindAddressToUse);
  

  ~ControlUnit();

  void run();

  void stop();

  std::thread start();


};
