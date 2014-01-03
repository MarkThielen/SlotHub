#include <string.h>
#include <stdlib.h>

#include "CarreraResponse.h"



//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
CarreraResponse::CarreraResponse(void *incoming_data, int length) { 

  
  response_type = UNKNOWN;
  
  memcpy(&responseData,incoming_data,length);

  if (get4Bits(responseData.car_number) >= CarreraResponse::CARRERA_MIN_CAR_NUMBER 
      && get4Bits(responseData.car_number) <= CarreraResponse::CARRERA_MAX_CAR_NUMBER)
    response_type = CAR_STATUS;
  
  else if (get4Bits(responseData.car_number) == CarreraResponse::CARRERA_TRACK_STATUS_FLAG)
    response_type = TRACK_STATUS;
  
  else
    response_type = UNKNOWN;
    
}

int CarreraResponse::getResponseType(){return response_type;}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
char CarreraResponse::getStartCode(){return responseData.start_code;}
    
//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
int CarreraResponse::getCarNumber(){return get4Bits(responseData.car_number);}


//------------------------------------------------------------------------
// - get4Bits - returns the lower 4 bits of the passed char as an integer
// -
// -----------------------------------------------------------------------
int CarreraResponse::get4Bits(char c) { return (int)(c & 0xf);}

    

//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
unsigned int CarreraResponse::getTimer(){
  return (get4Bits(responseData.data.scl.timer[1]) << 28) + (get4Bits(responseData.data.scl.timer[0]) << 24) +
    (get4Bits(responseData.data.scl.timer[3]) << 20) + (get4Bits(responseData.data.scl.timer[2]) << 16) +
    (get4Bits(responseData.data.scl.timer[5]) << 12) + (get4Bits(responseData.data.scl.timer[4]) << 8) +
    (get4Bits(responseData.data.scl.timer[7]) << 4) + (get4Bits(responseData.data.scl.timer[6]));
}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
unsigned int CarreraResponse::getCarFuelStatus(int carNumber){
  get4Bits(responseData.data.scts.fuel_status[carNumber - 1]);
}


// function that indicates that passed car number is currently in pits
// returns 1 if true, 0 otherwise
bool CarreraResponse::carInPits(int car_number) {
  return (((get4Bits((responseData.data.scts.fuel_bitmask[1]) << 4)) +
	  get4Bits((responseData.data.scts.fuel_bitmask[0]) & car_number)) >> (car_number-1));
}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
unsigned int CarreraResponse::getFuelMode(){return get4Bits(responseData.data.scts.fuel_mode) & 3;}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
unsigned int CarreraResponse::getStartLightStatus(){return get4Bits(responseData.data.scts.start_light_status);}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
bool CarreraResponse::getPitLaneInstalled() {return (((get4Bits(responseData.data.scts.fuel_mode) & 4) >> 2) > 0);}


//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
bool CarreraResponse::getLapCounterInstalled() {return ( (get4Bits((responseData.data.scts.fuel_mode) & 8) >> 3) > 0 );} 



//------------------------------------------------------------------------
// -
// -
// -----------------------------------------------------------------------
unsigned int CarreraResponse::getPitLaneState(){
  return (get4Bits((responseData.data.scts.fuel_bitmask[1]) << 4)) +
    get4Bits((responseData.data.scts.fuel_bitmask[0]));
}
