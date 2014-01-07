#pragma once

#include "pb/TrackStatusMessage.pb.h"


class TrackStatus {


 private:
  unsigned int start_lights_status;
  unsigned int fuel_mode;
  unsigned int inPit;
  bool pitlane_installed;
  bool lap_counter_installed;
  

  SlotHub::TrackStatusMessage *tsm;

 public:

  TrackStatus();
  ~TrackStatus();

  unsigned int getStartLightStatus();
  void setStartLightStatus(unsigned int status);

  unsigned int getFuelMode();
  void setFuelMode(unsigned int mode);

  bool getPitLaneInstalled();
  void setPitLaneInstalled(bool state);

  bool getLapCounterInstalled();
  void setLapCounterInstalled(bool state);

  unsigned int getInPit();
  void setInPit(unsigned int pitState);

};
