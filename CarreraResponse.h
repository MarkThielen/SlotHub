class CarreraResponse {

 public:
  enum type_response_type {UNKNOWN=0, CAR_STATUS, TRACK_STATUS};
 

 private:

  type_response_type response_type;

  static const int CARRERA_MIN_CAR_NUMBER = 0x1;
  static const int CARRERA_MAX_CAR_NUMBER = 0x8;
  static const int  CARRERA_TRACK_STATUS_FLAG = 0xa;

  struct ResponseData {
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

    }data;

  }responseData;

  
 public:

    CarreraResponse(void *data, int length);

    int getResponseType();

    char getStartCode();
    
    int getCarNumber();
    
    unsigned int getTimer();

    unsigned int getCarFuelStatus(int car_number);

    bool carInPits(int car_number);

    unsigned int getFuelMode();

    unsigned int getStartLightStatus();

    bool getPitLaneInstalled();

    bool getLapCounterInstalled();

    unsigned int getPitLaneState();

 private:
    // return lower 4 bits of char as integer
    int get4Bits(char c);


};
