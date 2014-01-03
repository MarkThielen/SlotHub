#include "TrackStatus.h"

unsigned int TrackStatus::getStartLightStatus(){return tsm->start_lights_status();}
void TrackStatus::setStartLightStatus(unsigned int status){tsm->set_start_lights_status (status);}

unsigned int TrackStatus::getFuelMode(){return tsm->fuel_mode();}
void TrackStatus::setFuelMode(unsigned int mode){tsm->set_fuel_mode(mode);}

bool TrackStatus::getPitLaneInstalled(){return tsm->pitlane_installed();}
void TrackStatus::setPitLaneInstalled(bool state){tsm->set_pitlane_installed(state);}

bool TrackStatus::getLapCounterInstalled(){return tsm->lap_counter_installed();}
void TrackStatus::setLapCounterInstalled(bool state){tsm->set_lap_counter_installed(state);}

unsigned int TrackStatus::getInPit(){return inPit;}
void TrackStatus::setInPit(unsigned int pitState) {inPit = pitState;}

TrackStatus::TrackStatus(){

  tsm = new SlotHub::TrackStatusMessage();
  
  setStartLightStatus(0);
  setFuelMode(0);
  setPitLaneInstalled(0);
  setLapCounterInstalled(0);
  setInPit(0);

}


TrackStatus::~TrackStatus(){

  delete(tsm);

}
