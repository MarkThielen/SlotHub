
// -------------------------------------------------------
// - data structures describing the different responses
// - received from the CU upon a "? command
// -------------------------------------------------------

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


// -------------------------------------------------------------
// - custom data structures
// -------------------------------------------------------------

struct STRUCT_CAR_STATUS {
  unsigned int car_number;
  unsigned int current_laptime;
  unsigned int fastest_laptime;
  unsigned int laps;
  unsigned int fuel_status;
  unsigned int active;
  unsigned int in_pit;
  unsigned int pits;
};

struct STRUCT_TRACK_STATUS {
  unsigned int lights_status;
  unsigned int fuel_mode;
  unsigned int pitlane_installed;
  unsigned int lapcounter_installed;
};

char CARRERA_COMMAND = 0x22;
char CARRERA_LAPINFO = 0x3f;

char CARRERA_MIN_CAR_NUMBER = 0x1;
char CARRERA_MAX_CAR_NUMBER = 0x8;
char CARRERA_TRACK_STATUS_FLAG = 0xa;
