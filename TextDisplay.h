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

  std::string serverAddress;


  void initDisplay();
  void refreshDisplay();
  void closeDisplay();
  
  void displayTrackStatus(TrackStatus *trackStatus);

  void displayCarTimings( std::map<int,CarStatus*> *mapCarStati );

  void initMessageQueue(std::string serverAddressToConnect);

  void displayStatusBar();
  
  CarStatus* receiveCarStatus();
  
  

  
 public:

  // -------------------------------------------------------------
  // - Constructor
  // -------------------------------------------------------------
  TextDisplay(std::string connectTo);


  
  // -------------------------------------------------------------
  // - Destructor
  // -------------------------------------------------------------
  ~TextDisplay();

  void setServerAddress(std::string connectTo);
  std::string getServerAddress();


  std::thread start();


  void run();


};
