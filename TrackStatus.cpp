#include "TrackStatus.h"

unsigned int TrackStatus::getStartLightStatus(){return start_lights_status;}
void TrackStatus::setStartLightStatus(unsigned int status){start_lights_status = status;}

unsigned int TrackStatus::getFuelMode(){return fuel_mode;}
void TrackStatus::setFuelMode(unsigned int mode){fuel_mode=mode;}

bool TrackStatus::getPitlaneInstalled(){return pitlane_installed;}
void TrackStatus::setPitLaneInstalled(bool state){pitlane_installed = state;}

bool TrackStatus::getLapCounterInstalled(){return lap_counter_installed;}
void TrackStatus::setLapCounterInstalled(bool state){lap_counter_installed = state;}
