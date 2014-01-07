#pragma once

#include <thread>
#include <map>

#include "CarStatus.h"
#include "TrackStatus.h"


class TextDisplay {

 private:
  
  void initDisplay();
  void refreshDisplay();
  void closeDisplay();
  
  void displayTrackStatus(TrackStatus *trackStatus);


  void displayCarTimings( std::map<int,CarStatus*> *mapCarStati  );
  
 public:

  std::thread start();

  void run();


};
