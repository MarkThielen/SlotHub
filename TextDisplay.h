#pragma once

#include <thread>
#include <map>
#include <zmq.hpp>

#include "CarStatus.h"
#include "TrackStatus.h"


class TextDisplay {

 private:
  
  zmq::context_t *context;
  zmq::socket_t *subscriber;

  std::map<int,CarStatus*> mapCarStati;


  void initDisplay();
  void refreshDisplay();
  void closeDisplay();
  
  void displayTrackStatus(TrackStatus *trackStatus);

  void displayCarTimings( std::map<int,CarStatus*> *mapCarStati );

  void initMessageQueue();

  
  CarStatus* receiveCarStatus();
  
  

  
 public:

  // -------------------------------------------------------------
  // - Constructor
  // -------------------------------------------------------------
  TextDisplay();


  
  // -------------------------------------------------------------
  // - Destructor
  // -------------------------------------------------------------
  ~TextDisplay();

  
  std::thread start();


  void run();


};
