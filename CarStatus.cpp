#include <CarStatus.h>

unsigned int getCarNumber() { return car_number;}
void setCarNumber(unsigned int carno){car_number = carno;}

unsigned int getCurrentLapTime(){return current_laptime;}
void setCurrentLapTime(unsigned int laptime){current_laptime = laptime;}

unsigned int getFastestLapTime(){return fastest_lap;}
void setFastestLapTime(unsigned int laptime){fastest_laptime = laptime;}

unsigned int getLaps(){return laps;}
void setLaps(unsigned int new_laps){laps = new_laps;}

unsigned int getFuelStatus(){return fuel_status;}
void setFuelStatus(unsigned int fuelStatus){fuel_status=fuelStatus;}

unsigned int getPitStops(){return pit_stops;}
void setPitStops(unsigned stops){pit_stops = stops;}
void incrementPitStops(){pit_stops++;}

bool getActive(){return active;}
void setActive(bool flag){active = flag;}
 
bool getInPit(){return in_pit;}
bool setInPit(bool flag){in_pit = flag;}





