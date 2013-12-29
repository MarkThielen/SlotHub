class TrackStatus {


 private:
  unsigned int start_lights_status;
  unsigned int fuel_mode;
  bool pitlane_installed;
  bool lap_counter_installed;


 public:

  unsigned int getStartLightStatus();
  void setStartLightStatus(unsigned int status);

  unsigned int getFuelMode();
  void setFuelMode(unsigned int mode);

  bool getPitlaneInstalled();
  void setPitLaneInstalled(bool state);

  bool getLapCounterInstalled();
  void setLapCounterInstalled(bool state);


};
