#include "CarreraResponse.h"





char CarreraResponse::getStartCode(){return start_code;}
    
char CarreraResponse::getCarNumber(){return car_number;}


int CarreraResponse::get4Bits(char c) { return (int)(c & 0xf);}
    
unsigned int CarreraResponse::getTimer(){
  return (get4Bits(data.scl.timer[1]) << 28) + (get4Bits(data.scl.timer[0]) << 24) +
    (get4Bits(data.scl.timer[3]) << 20) + (get4Bits(data.scl.timer[2]) << 16) +
    (get4Bits(data.scl.timer[5]) << 12) + (get4Bits(data.scl.timer[4]) << 8) +
    (get4Bits(data.scl.timer[7]) << 4) + (get4Bits(data.scl.timer[6]));
}



unsigned int CarreraResponse::getCarFuelStatus(int carNumber){return 0;}


unsigned int CarreraResponse::getFuelMode(){return data.scts.fuel_mode;}


unsigned int CarreraResponse::getStartLightStatus(){return data.scts.start_light_status;}
