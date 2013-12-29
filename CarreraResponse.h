class CarreraResponse {

 private:
    char start_code;
    char car_number;

    union DATA {

      struct STRUCT_CARRERA_LAPINFO {
	char timer[8];
	char group;
	
	char checksum;
	char end_code;
      }scl;

      struct STRUCT_CARRERA_TRACK_STATUS {
	char fuel_status[8];
	char start_light_status;
	char fuel_mode;
	char fuel_bitmask[2];
	char tower_mode;
	
	char checksum;
	char end_code;
      }scts;

    } data;


 public:

    char getStartCode();
    
    char getCarNumber();
    
    unsigned int getTimer();



    unsigned int getCarFuelStatus(int carNumber);

    unsigned int getFuelMode();

    unsigned int getStartLightStatus();


}
