#include "CarStatus.h"

// --------------------------------------------------------------
// - CarNumber
// -
// --------------------------------------------------------------
unsigned int CarStatus::getCarNumber() { return csm->car_number();}
void CarStatus::setCarNumber(unsigned int carno){csm->set_car_number(carno);}

// --------------------------------------------------------------
// - CurrentLapTime
// -
// --------------------------------------------------------------
unsigned int CarStatus::getCurrentLapTime(){return csm->current_laptime();}
void CarStatus::setCurrentLapTime(unsigned int laptime){csm->set_current_laptime(laptime); }


// --------------------------------------------------------------
// - FastestLapTime
// -
// --------------------------------------------------------------
unsigned int CarStatus::getFastestLapTime(){return csm->fastest_laptime();}
void CarStatus::setFastestLapTime(unsigned int laptime){csm->set_fastest_laptime(laptime);}

// --------------------------------------------------------------
// - Laps
// -
// --------------------------------------------------------------
unsigned int CarStatus::getLaps(){return csm->laps();}
void CarStatus::setLaps(unsigned int new_laps){csm->set_laps(new_laps);}
void CarStatus::incrementLaps(){csm->set_laps(csm->laps() +1 );}

// --------------------------------------------------------------
// - FuelStatus
// -
// --------------------------------------------------------------
unsigned int CarStatus::getFuelStatus(){return csm->fuel_status();}
void CarStatus::setFuelStatus(unsigned int fuelStatus){csm->set_fuel_status(fuelStatus);}


// --------------------------------------------------------------
// - PitStops
// -
// --------------------------------------------------------------
unsigned int CarStatus::getPitStops(){return csm->pit_stops();}
void CarStatus::setPitStops(unsigned stops){csm->set_pit_stops(stops);}
void CarStatus::incrementPitStops(){csm->set_pit_stops(csm->pit_stops() + 1);}


// --------------------------------------------------------------
// - Active
// -
// --------------------------------------------------------------
bool CarStatus::getActive(){return csm->active();}
void CarStatus::setActive(bool flag){csm->set_active(flag);}



// --------------------------------------------------------------
// - InPits
// - 
// -------------------------------------------------------------- 
bool CarStatus::getInPit(){return csm->in_pit();}
bool CarStatus::setInPit(bool currentlyInPit){csm->set_in_pit(currentlyInPit);}


// --------------------------------------------------------------
// - updatePitStopStatistics - does set the InPits indicator and
// - updates the number of pit stops during the session
// -------------------------------------------------------------- 
void CarStatus::updatePitStopStatistics(bool currentlyInPit) {

  // when car has exited the pit lane increment the pit stop counter
  if (getInPit() && !currentlyInPit)
    incrementPitStops();

  // set pit stop indicator
  setInPit(currentlyInPit);

}


// --------------------------------------------------------------
// - PreviousTimer
// -
// --------------------------------------------------------------
unsigned int CarStatus::getPreviousTimer(){return previous_timer;}
void CarStatus::setPreviousTimer(unsigned int timer) {previous_timer = timer;}


// --------------------------------------------------------------
// -
// -
// --------------------------------------------------------------
void CarStatus::updateTimeAndLapStatistics(unsigned int timer) {

  // calculate the current lap time
  if (getPreviousTimer() > 0)
    setCurrentLapTime(timer - getPreviousTimer());

  setPreviousTimer(timer);

  
  if (getFastestLapTime() == 0)
    setFastestLapTime(getCurrentLapTime());
  // update fastest lap time if current was faster than the previously 
  // recorded fastest lap time
  else if (getFastestLapTime() > 0 && getFastestLapTime() > getCurrentLapTime())
    setFastestLapTime(getCurrentLapTime());
  
  incrementLaps();

}


// --------------------------------------------------------------
// -
// -
// --------------------------------------------------------------
CarStatus::CarStatus(unsigned int new_car_number) {

  
  // init protocol buffer message
  csm = new SlotHub::CarStatusMessage();

  // initialize timer
  setPreviousTimer(0);

  // set Car Number
  setCarNumber(new_car_number);
  
  // reset other information
  setCurrentLapTime(0);
  setFastestLapTime(0);
  setLaps(0);
  setFuelStatus(0);
  setPitStops(0);
  setActive(true);
  setInPit(false);
  
}

// --------------------------------------------------------------
// -
// -
// --------------------------------------------------------------
CarStatus::~CarStatus() {

  delete(csm);

}
