#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <cstddef>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <map>

#include <ncurses.h>
#include <argtable2.h>

#include "ttyTools.h"
#include "CarStatus.h"
#include "TrackStatus.h"
#include "CarreraResponse.h"

char *portname = "/dev/ttyUSB0";
char *CARRERA_TIMING_QUERY = "\"?";
char *CARRERA_CU_FIRMWARE = "\"0";




void initDisplay() {
  WINDOW *mainWindow = initscr();
  start_color();
  init_pair(1,COLOR_WHITE,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLACK);
  init_pair(3,COLOR_RED,COLOR_BLACK);
  init_pair(4,COLOR_YELLOW,COLOR_BLACK);

  wbkgd(mainWindow,COLOR_PAIR(1));

  raw();
  noecho();
  timeout(10);
}

void refreshDisplay() {
  refresh();
}

void closeDisplay() {
  endwin();
}



// -------------------------------------
// Display track status information
// -------------------------------------
void displayTrackStatus(TrackStatus *pTrackStatus) {

 // left border and header position
  int left_border = 5, 
    header_top = 15;

  int col_light_status = 0, 
    col_fuel_mode = 25, 
    col_pitlane_installed = 40, 
    col_lapcounter_installed = 60,
    col_inPit = 75;
  

  // print header
  attron(A_BOLD);
  mvprintw(header_top,left_border + col_light_status,"(Lights Status) %u", pTrackStatus->getStartLightStatus());
  mvprintw(header_top,left_border + col_fuel_mode,"(Fuel Mode) %u",pTrackStatus->getFuelMode());  
  mvprintw(header_top,left_border + col_pitlane_installed,"(Pitlane) %u",pTrackStatus->getPitLaneInstalled()); 
  mvprintw(header_top,left_border + col_lapcounter_installed,"(Lapcounter) %u",pTrackStatus->getLapCounterInstalled());
  mvprintw(header_top,left_border + col_inPit,"(InPit) %u",pTrackStatus->getInPit());


  attroff(A_BOLD);

}


// -------------------------------------------
// display car times and status
// -------------------------------------------
void displayTimes(std::map<int,CarStatus*> *mapCarStati) {

  // left border and header position
  int left_border = 5, 
    header_top = 1;
  
  // info column definition
  int col_car_number=0, 
    col_current_lap = 5, 
    col_fastest_lap=15, 
    col_diff=25, 
    col_laps=35, 
    col_pits=45,
    col_fuel_status=55;

  // print header
  attron(A_BOLD);
  mvprintw(header_top,left_border,"Car#");
  mvprintw(header_top,left_border + col_current_lap,"Current");
  mvprintw(header_top,left_border + col_fastest_lap,"Fastest");
  mvprintw(header_top,left_border + col_diff,"Diff");
  mvprintw(header_top,left_border + col_laps,"Laps");
  mvprintw(header_top,left_border + col_pits,"Pits");
  mvprintw(header_top,left_border + col_fuel_status,"0------50------100");
  attroff(A_BOLD);

  // print car list
  for (std::map<int,CarStatus*>::iterator iterCarStati = mapCarStati->begin() ; iterCarStati != mapCarStati->end(); ++iterCarStati)
      {
	
	CarStatus *currentCar = iterCarStati->second;

	int carno = currentCar->getCarNumber();
	

	mvprintw(header_top + carno + 1,left_border + col_car_number,"%u",currentCar->getCarNumber());

	// show personal best in green
	if (currentCar->getCurrentLapTime() > 0 &&  (currentCar->getCurrentLapTime() == currentCar->getFastestLapTime()))
	  attron(COLOR_PAIR(2));

	mvprintw(header_top + carno + 1,left_border + col_current_lap,"%u", currentCar->getCurrentLapTime());
	mvprintw(header_top + carno + 1,left_border + col_fastest_lap,"%u", currentCar->getFastestLapTime());
	
	// turn off personal best in green
	attroff(COLOR_PAIR(2));
	mvprintw(header_top + carno + 1,left_border + col_diff,"%u",(currentCar->getCurrentLapTime() - currentCar->getFastestLapTime()));
	mvprintw(header_top + carno + 1,left_border + col_laps,"%u",currentCar->getLaps());
	mvprintw(header_top + carno + 1,left_border + col_pits,"%u",currentCar->getPitStops());
	
	//mvprintw(header_top + carno + 1,left_border + 90,"%u",currentCar->getFuelStatus());

	// -------------------------------------------------------
	// show fuel bar 0%-100% with moving current
	// -------------------------------------------------------
	int fuel_color_pair = 1;
	// if fuel is 15-8 show green
	if (currentCar->getFuelStatus() >=8)
	  fuel_color_pair = 2;
	// if fuel 7-2 show red fuel bars
	else if (currentCar->getFuelStatus() <= 7 && currentCar->getFuelStatus() >= 2 )
	  fuel_color_pair = 4;
	// if fuel <= 1 show red fuel bars
	else if (currentCar->getFuelStatus() <= 1)
	  fuel_color_pair = 3;

	attron(COLOR_PAIR(fuel_color_pair));
	
	int fuelbars=0;
	for (;fuelbars <= 15;fuelbars++)
	  mvprintw(header_top + carno + 1, left_border + col_fuel_status + fuelbars,"-");
	
	mvprintw(header_top + carno + 1, left_border + col_fuel_status + currentCar->getFuelStatus() ,"+");
	
	attroff(COLOR_PAIR(fuel_color_pair));

      }
  
}



// -------------------------------------------------------------
//
// -------------------------------------------------------------




int main (int argc, char **argv){


  // -----------------------------------------------------
  // - use argtable 2 to define / check / retrieve 
  // - cmd line parameters
  // -----------------------------------------------------
  // argtable structure definitions
  struct arg_file *ttyDevice = arg_file0("d","device",NULL,"Serial Terminal");
  struct arg_end *end = arg_end(20);

  

  void *argtable[] = { 
    ttyDevice, end      
  };

  // check argtable
  if (arg_nullcheck(argtable) != 0)
    printf ("error: insufficient memory\n");

  // set default value for tty device to use
  // will be overwritten if something was passed
  // on the cmd line
  ttyDevice->filename[0] = "/dev/ttyUSB0";


  // parse argtable
  int parse_errors = arg_parse(argc,argv,argtable);

  if(parse_errors > 0)
    arg_print_errors(stdout,end,"SlotHub");

  // --------------------------------------------------------
  // open tty
  // --------------------------------------------------------
  int fd = open (ttyDevice->filename[0], O_RDWR | O_NOCTTY | O_SYNC);

 // display error if open no successful
 if (fd < 0)
   {
     printf        ("error %d opening %s: %s\n", errno, ttyDevice->filename[0], strerror (errno));
     return -1;
   }
 
 // set serial connection parameters
 set_interface_attribs (fd, B19200 , 0);  // set speed to 115,200 bps, 8n1 (no parity)
 set_blocking (fd, 0);                // set no blocking

 unsigned int prev_timer=0;
 unsigned int laps=0;

  char *buffer = (char *)malloc(sizeof (struct CarreraResponse::ResponseData));

  // use map to store current car stati
  std::map<int,CarStatus*> mapCarStati;

  // Track Status object
  TrackStatus *trackStatus = new TrackStatus(); 


  
 initDisplay();

 while (getch() != 'q'){

   unsigned int timer = 0, laptime = 0;

   // send request to CU
   write (fd, CARRERA_TIMING_QUERY, strlen(CARRERA_TIMING_QUERY));       

   // reset memory structure
   memset(buffer,0,sizeof(struct CarreraResponse::ResponseData));

   // read data from tty
   int n = read (fd, buffer, sizeof (struct CarreraResponse::ResponseData) + 1);

   CarreraResponse cr = CarreraResponse(buffer,n);

   //printf("Buffer : %s : %i %c \n",carrera_response,n, carrera_response->car_number );

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


   usleep (50000);

   displayTimes(&mapCarStati);
   displayTrackStatus(trackStatus);

   refreshDisplay();

 }

 closeDisplay();
 
 free(buffer);

 return 0;

} 






