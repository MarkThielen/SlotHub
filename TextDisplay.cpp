#include <thread>
#include <map>
#include <ncurses.h>

#include "TextDisplay.h"
#include "CarStatus.h"
#include "TrackStatus.h"



void TextDisplay::initDisplay() {
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

void TextDisplay::refreshDisplay() {
  refresh();
}

void TextDisplay::closeDisplay() {
  endwin();
}



// -------------------------------------
// Display track status information
// -------------------------------------
void TextDisplay::displayTrackStatus(TrackStatus *pTrackStatus) {

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
void TextDisplay::displayCarTimings(std::map<int,CarStatus*> *mapCarStati) {

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



