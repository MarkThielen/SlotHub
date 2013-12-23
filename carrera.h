
 struct STRUCT_CARRERA_LAPINFO {
    char timer[8];
    char group;
    char checksum;
    char end_code;
  };

  struct STRUCT_CARRERA_TRACK_STATUS {
    char fuel_status[8];
    char start_light_status;
    char fuel_mode;
    char fuel_bitmask[2];
    char tower_mode;
    char checksum;
    char end_code;
  };


  struct STRUCT_CARRERA_RESPONSE {
    char start_code;
    char car_number;

    union DATA {
      struct STRUCT_CARRERA_LAPINFO scl;
      struct STRUCT_CARRERA_TRACK_STATUS scts;
    } data;
 
  };


char CARRERA_COMMAND = 0x22;
char CARRERA_LAPINFO = 0x3f;

char CARRERA_MIN_CAR_NUMBER = 0x1;
char CARRERA_MAX_CAR_NUMBER = 0x8;
char CARRERA_TRACK_STATUS_FLAG = 0xa;
