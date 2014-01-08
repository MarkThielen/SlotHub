#pragma once

#include <string>
#include <thread>

#include <zmq.hpp>

#include "CarStatus.h"
#include "TrackStatus.h"


class ControlUnit {




 private:
  
  std::string tty;
  
  int fileDescriptor;

  zmq::context_t *context;
  zmq::socket_t *publisher;
  
  int set_interface_attribs (int fd, int speed, int parity);
  void set_blocking (int fd, int should_block);

  void initMessageQueue();

  void sendCarStatusMessage(SlotHub::CarStatusMessage *csm);
  void sendTrackStatusMessage( SlotHub::TrackStatusMessage *tsm  );


 public:

  ControlUnit(std::string ttyToUse);

  int getFileDescriptor();
  

  ~ControlUnit();

  void run();

  std::thread start();


};
