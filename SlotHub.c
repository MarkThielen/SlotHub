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

#include <ncurses.h>

#include <argtable2.h>

#include "carrera.h"
#include "ttyTools.h"

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
void displayTrackStatus(struct STRUCT_TRACK_STATUS *pTrackStatus) {

 // left border and header position
  int left_border = 5, 
    header_top = 15;

  int col_light_status = 0, col_fuel_mode = 25, col_pitlane_installed = 40, col_lapcounter_installed = 60;

  // print header
  attron(A_BOLD);
  mvprintw(header_top,left_border + col_light_status,"(Light Status) %u" + pTrackStatus->lights_status);
  mvprintw(header_top,left_border + col_fuel_mode,"(Fuel Mode) %u",pTrackStatus->fuel_mode);  
  mvprintw(header_top,left_border + col_pitlane_installed,"(Pitlane) %u",pTrackStatus->pitlane_installed); 
  mvprintw(header_top,left_border + col_lapcounter_installed,"(Lapcounter) %u",pTrackStatus->lapcounter_installed);


  attroff(A_BOLD);

}

// -------------------------------------------
// display car times and status
// -------------------------------------------
void displayTimes(struct STRUCT_CAR_STATUS *car_stati) {

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
  int carno=1;
  for (carno=1; carno <= 6; carno ++)
      {

	mvprintw(header_top + carno + 1,left_border + col_car_number,"%u",car_stati[carno].car_number);

	// show personal best in green
	if (car_stati[carno].current_laptime > 0 &&  (car_stati[carno].current_laptime == car_stati[carno].fastest_laptime))
	  attron(COLOR_PAIR(2));

	mvprintw(header_top + carno + 1,left_border + col_current_lap,"%u",car_stati[carno].current_laptime);
	mvprintw(header_top + carno + 1,left_border + col_fastest_lap,"%u",car_stati[carno].fastest_laptime);
	
	// turn off personal best in green
	attroff(COLOR_PAIR(2));
	mvprintw(header_top + carno + 1,left_border + col_diff,"%u",(car_stati[carno].current_laptime-car_stati[carno].fastest_laptime));
	mvprintw(header_top + carno + 1,left_border + col_laps,"%u",car_stati[carno].laps);
	mvprintw(header_top + carno + 1,left_border + col_pits,"%u",car_stati[carno].pits);
	
	//mvprintw(header_top + carno + 1,left_border + 60,"%u",car_stati[carno].fuel_status);

	// -------------------------------------------------------
	// show fuel bar 0%-100% with moving current
	// -------------------------------------------------------
	int fuel_color_pair = 1;
	// if fuel is 15-8 show green
	if (car_stati[carno].fuel_status >=8)
	  fuel_color_pair = 2;
	// if fuel 7-2 show red fuel bars
	else if (car_stati[carno].fuel_status <= 7 && car_stati[carno].fuel_status >= 2 )
	  fuel_color_pair = 4;
	// if fuel <= 1 show red fuel bars
	else if (car_stati[carno].fuel_status <= 1)
	  fuel_color_pair = 3;

	attron(COLOR_PAIR(fuel_color_pair));
	
	int fuelbars=0;
	for (;fuelbars <= 15;fuelbars++)
	  mvprintw(header_top + carno + 1, left_border + col_fuel_status + fuelbars,"-");
	mvprintw(header_top + carno + 1, left_border + col_fuel_status + car_stati[carno].fuel_status ,"+");
	
	attroff(COLOR_PAIR(fuel_color_pair));

      }
  
}




// return lower 4 bits of char as integer
int get4Bits(char c) { return (int)(c & 0xf);}

// convert Carrera CU timer information into an integer value
// holding ms
unsigned int getTimer (struct STRUCT_CARRERA_LAPINFO scl ) {
  return 
    (get4Bits(scl.timer[1]) << 28) +
    (get4Bits(scl.timer[0]) << 24) +
    (get4Bits(scl.timer[3]) << 20) +
    (get4Bits(scl.timer[2]) << 16) +
    (get4Bits(scl.timer[5]) << 12) +
    (get4Bits(scl.timer[4]) << 8) +
    (get4Bits(scl.timer[7]) << 4) +
    (get4Bits(scl.timer[6]));
}

// function that indicates that passed car number is currently in pits
// returns 1 if true, 0 otherwise
int inPits(struct STRUCT_CARRERA_RESPONSE *carrera_response, int car_number) {
  return ((get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[1]) << 4) + 
	  get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[0]) & car_number) >> (car_number-1);
}


// reset car stati
void resetCarStati(struct STRUCT_CAR_STATUS car_stati[]) {

int carno=0;
 for (carno=0; carno <= 7; carno ++){
   car_stati[carno].car_number=carno;
   car_stati[carno].active=0;
   car_stati[carno].current_laptime=0;
   car_stati[carno].fastest_laptime=0;
   car_stati[carno].laps=0;
   car_stati[carno].fuel_status=0;
   car_stati[carno].in_pit=0;
   car_stati[carno].pits=0;
 }

}

// -------------------------------------------------------------
//
// -------------------------------------------------------------




void main (int argc, char **argv){


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
     return;
   }
 
 // set serial connection parameters
 set_interface_attribs (fd, B19200 , 0);  // set speed to 115,200 bps, 8n1 (no parity)
 set_blocking (fd, 0);                // set no blocking

 unsigned int prev_timer=0;
 unsigned int laps=0;


  // Carrera CU response structure that stores all
  // response data coming back from the CU
  struct STRUCT_CARRERA_RESPONSE *carrera_response = 
    (struct STRUCT_CARRERA_RESPONSE *)malloc(sizeof (struct STRUCT_CARRERA_RESPONSE));

  // create array of cars on track
  struct STRUCT_CAR_STATUS car_stati[8];
  resetCarStati(car_stati);


  // create track status
  struct STRUCT_TRACK_STATUS *track_status =(struct STRUCT_TRACK_STATUS *)malloc(sizeof (struct STRUCT_TRACK_STATUS));
  
  track_status->lights_status = 0;
  track_status->fuel_mode = 0;
  track_status->pitlane_installed = 0;
  track_status->lapcounter_installed = 0;


 initDisplay();

 while (getch() != 'q'){

   unsigned int timer = 0, laptime = 0;

   // send request to CU
   write (fd, CARRERA_TIMING_QUERY, strlen(CARRERA_TIMING_QUERY));       

   // reset memory structure
   memset(carrera_response,0,sizeof(struct STRUCT_CARRERA_RESPONSE));

   // read data from tty
   int n = read (fd, carrera_response, sizeof (struct STRUCT_CARRERA_RESPONSE) + 1);

   //printf("Buffer : %s : %i %c \n",carrera_response,n, carrera_response->car_number );


   
   // -----------------------------------------------------------------------
   // - Calculate and show laptime of last car crossing the finish line 
   // -----------------------------------------------------------------------
   if (get4Bits(carrera_response->car_number) >= CARRERA_MIN_CAR_NUMBER 
       && get4Bits(carrera_response->car_number) <= CARRERA_MAX_CAR_NUMBER) {

     unsigned int car_number = get4Bits(carrera_response->car_number);

     // set car information in car status array
     if ( car_stati[car_number].active == 0 )
       car_stati[car_number].active = 1;

     car_stati[car_number].laps++;

     
     // set car number
     car_stati[car_number].car_number = car_number;

     // get current CU timer
     timer = getTimer (((union DATA)carrera_response->data).scl);

     // -----------------------------------------------------
     // previous timer should be set after one lap, if yes
     // we are able to calculate a meaningful lap time
     // -----------------------------------------------------
     if(prev_timer > 0) {
       
       // laptime is substracting the current timer with the previous
       // timer
       laptime = timer - prev_timer;
 
       // set current laptime
       car_stati[car_number].current_laptime = laptime;

       // if last lap time was fastest set fastest laptime to current 
       if ( (car_stati[car_number].current_laptime < 
	     car_stati[car_number].fastest_laptime) || car_stati[car_number].fastest_laptime == 0 )
	 car_stati[car_number].fastest_laptime = 
	   car_stati[car_number].current_laptime;
       
       
       /* // show Car number, current timer and laptime */
       /* printf ("Car #%c T(I): %u LT = %u \n", */
       /* 	  carrera_response->car_number, timer,laptime); */

     }
     // ----------------------------------------------------------
     // set previous time to current to calculate next lap time
     // ----------------------------------------------------------
     prev_timer = timer;
   } 

   // ---------------------------------------------------------------
   // - collect tank mode & status, start light status, tower mode 
   // ---------------------------------------------------------------
   else if (get4Bits(carrera_response->car_number) == CARRERA_TRACK_STATUS_FLAG) {

     // set fuel status of cars 1-6
     // set in pit flag to indicate that car is in pits
     int count=1;
     for (;count <=6;count ++){ 
	 car_stati[count].fuel_status = get4Bits(((union DATA)carrera_response->data).scts.fuel_status[count-1]);

	 // if car was in pit stop and has exited
	 // increment the pits counter
	 if (car_stati[count].in_pit == 1 && !inPits(carrera_response,car_stati[count].car_number))
	   car_stati[count].pits++;

	 car_stati[count].in_pit = inPits(carrera_response, car_stati[count].car_number) ;	     	 
       }

     // set track information (light status, fuel mode)
     track_status->lights_status = get4Bits(((union DATA)carrera_response->data).scts.start_light_status);
     track_status->fuel_mode =  get4Bits(((union DATA)carrera_response->data).scts.fuel_mode) & 3;

     // check for installed equipment (pitlane, lapcounter)
     track_status->pitlane_installed =  (get4Bits(((union DATA)carrera_response->data).scts.fuel_mode) & 4) >> 2;
     track_status->lapcounter_installed =  (get4Bits(((union DATA)carrera_response->data).scts.fuel_mode) & 8) >> 3;     

   }

   usleep (50000);

   displayTimes(car_stati);

   displayTrackStatus(track_status);

   refreshDisplay();

 }

 free(track_status);

 closeDisplay();

} 






