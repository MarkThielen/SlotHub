#include "ControlUnit.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <zmq.hpp>

#include <exception>
#include <map>
#include <thread>


#include "CarreraResponse.h"
#include "CarStatus.h"
#include "TrackStatus.h"


// -------------------------------------------------------------
// - Constructor
// -------------------------------------------------------------
ControlUnit::ControlUnit(std::string ttyToUse){


  fileDescriptor = -1;

  tty = std::string(ttyToUse);

  

  // --------------------------------------------------------
  // open tty
  // --------------------------------------------------------
  fileDescriptor = open (tty.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

  // display error if open no successful
  if (fileDescriptor < 0)
    {
      printf        ("error %d opening %s: %s\n", errno, tty.c_str(), strerror (errno));
      throw std::exception();
    }
  
  // set serial connection parameters
  set_interface_attribs (fileDescriptor, B19200 , 0);  // set speed to 115,200 bps, 8n1 (no parity)
  set_blocking (fileDescriptor, 0);                // set no blocking

  // init the message queue
  initMessageQueue();

}

// -------------------------------------------------------------
// - Destructor
// -------------------------------------------------------------
ControlUnit::~ControlUnit(){

  close(fileDescriptor);

  delete(publisher);
  delete(context);

}

// -------------------------------------------------------------
// - Init Message Queue
// -------------------------------------------------------------
void ControlUnit::initMessageQueue() {

  context = new zmq::context_t(1);
  publisher = new zmq::socket_t(*context, ZMQ_PUB);

  publisher->bind("tcp://*:5556");
  publisher->bind("ipc://control_unit.ipc");

}


// -------------------------------------------------------------
// - send Car Status Message to Queue
// -------------------------------------------------------------
void ControlUnit::sendCarStatusMessage(SlotHub::CarStatusMessage *csm) {

  char buffer[1024];

  csm->SerializeToArray(buffer,csm->ByteSize());


  publisher->send(buffer,csm->ByteSize());

}


// -------------------------------------------------------------
// - send Track Status Message to Queue
// -------------------------------------------------------------
void ControlUnit::sendTrackStatusMessage(SlotHub::TrackStatusMessage *tsm) {

  char buffer[1024];

  tsm->SerializeToArray(buffer,tsm->ByteSize());

  publisher->send(buffer,tsm->ByteSize());


}



// -------------------------------------------------------------
// - Return file descriptor connecting to CU
// -------------------------------------------------------------
int ControlUnit::getFileDescriptor() { 

  if (fileDescriptor <= 0) 
    throw std::exception();

  return fileDescriptor; 

}


// -------------------------------------------------------------
// - create a thread and start the main logic
// -------------------------------------------------------------
std::thread ControlUnit::start() {

  return std::thread([this]{this->run();});

}


// -------------------------------------------------------------
// - main loop logic
// -------------------------------------------------------------
void ControlUnit::run(){

  
  std::string CARRERA_TIMING_QUERY = std::string("\"?");
  std::string CARRERA_CU_FIRMWARE = std::string("\"0");

  std::chrono::duration<int, std::milli> sleep_duration(66000);

  unsigned int prev_timer=0;
  unsigned int laps=0;
  
  char *buffer = (char *)malloc(sizeof (struct CarreraResponse::ResponseData));

  // use map to store current car stati
  std::map<int,CarStatus*> mapCarStati;
  
  // Track Status object
  TrackStatus *trackStatus = new TrackStatus(); 


  while (true){

    unsigned int timer = 0, laptime = 0;

    // send request to CU
    write (getFileDescriptor(), CARRERA_TIMING_QUERY.c_str(), CARRERA_TIMING_QUERY.size());       
    
    // reset memory structure
    memset(buffer,0,sizeof(struct CarreraResponse::ResponseData));
    
    // read data from tty
    int n = read (getFileDescriptor(), buffer, sizeof (struct CarreraResponse::ResponseData) + 1);
    
    CarreraResponse cr = CarreraResponse(buffer,n);

    // -----------------------------------------------------------------------
    // - Calculate and show laptime of last car crossing the finish line 
    // -----------------------------------------------------------------------
    if (cr.getResponseType() == CarreraResponse::CAR_STATUS) {
      
      // find car in current map of cars
      std::map<int,CarStatus*>::iterator iterCarStatus = mapCarStati.find(cr.getCarNumber()); 
      
      // update data for already active car
      if(iterCarStatus != mapCarStati.end())
	iterCarStatus->second->updateTimeAndLapStatistics(cr.getTimer());
      // ----------------------------------------------
      // set initial data for new car on track
      // ---------------------------------------------
      else 
        // add new car to list of available cars
	mapCarStati.insert(std::pair<int, CarStatus*>(cr.getCarNumber(), new CarStatus(cr.getCarNumber())));
      
    } 
    // ---------------------------------------------------------------
    // - collect fuel mode & status, start light status, tower mode 
    // ---------------------------------------------------------------
    else if (cr.getResponseType() == CarreraResponse::TRACK_STATUS) {
      
      for (std::map<int,CarStatus*>::iterator iterCarStatus = mapCarStati.begin(); iterCarStatus != mapCarStati.end(); ++iterCarStatus) {
	
        CarStatus *currentCar = iterCarStatus->second;
	
	// update fuel status
	currentCar->setFuelStatus(cr.getCarFuelStatus(currentCar->getCarNumber()));
	
	// update PitStop Statistics
	currentCar->updatePitStopStatistics(cr.carInPits(currentCar->getCarNumber()));
	
      }
      
      // ------------------------------------------------------
      // set track information (light status, fuel mode)
      // ------------------------------------------------------ 
      trackStatus->setStartLightStatus(cr.getStartLightStatus());
      trackStatus->setFuelMode(cr.getFuelMode());

      
      // ------------------------------------------------------
      // check for installed equipment (pitlane, lapcounter)
      // ------------------------------------------------------
      trackStatus->setPitLaneInstalled(cr.getPitLaneInstalled());
      trackStatus->setLapCounterInstalled(cr.getLapCounterInstalled());
      
      trackStatus->setInPit(cr.getPitLaneState());
      
    }

    // ---------------------------------------------------
    // - send updated car information to message queue
    // ---------------------------------------------------
    for (std::map<int,CarStatus*>::iterator iterCarStatus = mapCarStati.begin(); iterCarStatus != mapCarStati.end(); ++iterCarStatus) 
      sendCarStatusMessage(iterCarStatus->second->getCarStatusMessage());
      
    // sleep a little
    std::this_thread::sleep_for(sleep_duration);
   

 }

}


// ---------------------------------------------------------------------------
// - Serial Communication tools
// ---------------------------------------------------------------------------
int ControlUnit::set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf        ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf        ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void ControlUnit::set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf        ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf        ("error %d setting term attributes", errno);
}


