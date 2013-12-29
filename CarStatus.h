class CarStatus {

private:
  unsigned int car_number;
  unsigned int current_laptime;
  unsigned int fastest_laptime;
  unsigned int laps;
  unsigned int fuel_status;
  bool active;
  bool in_pit;
  unsigned int pit_stops;

public:

  unsigned int getCarNumber();
  void setCarNumber(unsigned int carno);

  unsigned int getCurrentLapTime();
  void setCurrentLapTime(unsigned int laptime);

  unsigned int getFastestLapTime();
  void setFastestLapTime(unsigned int laptime);

  unsigned int getLaps();
  void setLaps(unsigned int laps);

  unsigned int getFuelStatus();
  void setFuelStatus(unsigned int fuelStatus);

  unsigned int getPitStops();
  void setPitStops(unsigned stops);
  void incrementPitStops();


  bool getActive();
  void setActive(bool flag);
 
  bool getInPit();
  bool setInPit(bool flag);



}



