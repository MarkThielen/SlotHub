#include <thread>
#include <map>
#include <ncurses.h>
#include <thread>

#include "TextDisplay.h"
#include "CarStatus.h"
#include "TrackStatus.h"






// -------------------------------------------------------------
// - create a thread and start the main logic
// -------------------------------------------------------------
std::thread TextDisplay::start() {

  return std::thread([this]{this->run();});

}


// -------------------------------------------------------------
// - main loop logic
// -------------------------------------------------------------
void TextDisplay::run(){

  initDisplay();

  while (getch() != 'q') {
   
    CarStatus *cs = receiveCarStatus();

    // find car in current map of cars
    std::map<int,CarStatus*>::iterator iterCarStatus = mapCarStati.find(cs->getCarNumber()); 
    
    // update data for already active car
    if(iterCarStatus != mapCarStati.end()) 
      iterCarStatus->second->updateCarStatus(cs);
    // ----------------------------------------------
    // set initial data for new car on track
    // ---------------------------------------------
    else 
      // add new car to list of available cars
      mapCarStati.insert(std::pair<int, CarStatus*>(cs->getCarNumber(), new CarStatus(cs)));
    
    // delete object
    delete(cs);

    // show car timings on screen
    displayCarTimings(&mapCarStati);


    // display status bar
    displayStatusBar();
    
    // refresh the display
    refreshDisplay();

  }
  // close display after exit
  closeDisplay();
}


std::string TextDisplay::getServerAddress() {return std::string(serverAddress);}

void TextDisplay::setServerAddress(std::string connectTo){serverAddress = std::string(connectTo);}


// -------------------------------------------------------------
// - Constructor
// -------------------------------------------------------------
TextDisplay::TextDisplay(std::string connectTo){

  setServerAddress(connectTo);
  

  initMessageQueue(getServerAddress());
  
  

}


// -------------------------------------------------------------
// - Destructor
// -------------------------------------------------------------
TextDisplay::~TextDisplay(){

  
}


// -------------------------------------------------------------
// - receive CarStatus from Message Queue
// -------------------------------------------------------------
CarStatus* TextDisplay::receiveCarStatus(){

  zmq::message_t msg_cardata;

  SlotHub::CarStatusMessage *csm = new SlotHub::CarStatusMessage();

  subscriber->recv(&msg_cardata);

  csm->ParseFromArray(msg_cardata.data(), msg_cardata.size());

  return new CarStatus(csm);

} 


// -------------------------------------------------------------
// - Init Message Queue
// -------------------------------------------------------------
void TextDisplay::initMessageQueue(std::string connectTo) {

  context = new zmq::context_t(1);
  subscriber = new zmq::socket_t(*context, ZMQ_SUB);

  subscriber->connect(getServerAddress().c_str());

  subscriber->setsockopt(ZMQ_SUBSCRIBE,NULL,0);



}


// -------------------------------------------------------------
// - Init Display
// -------------------------------------------------------------
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

  // display status bar
  displayStatusBar();

  // refresh Display
  refreshDisplay();

}


// -------------------------------------------------------------
// - refresh display
// -------------------------------------------------------------
void TextDisplay::refreshDisplay() {
  refresh();
}

// -------------------------------------------------------------
// - Close Display
// -------------------------------------------------------------
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
// - show some status information at the 
// - bottom of the screen
// -------------------------------------------
void TextDisplay::displayStatusBar() {

  int rows, cols;
  
  getmaxyx(stdscr,rows,cols);
  
  mvprintw(rows-1,0,"Server : %s", getServerAddress().c_str());
  mvprintw(rows-1,cols-12,"'q' to exit", getServerAddress().c_str());

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

	mvprintw(header_top + carno + 1,left_border + col_current_lap,"%-6u", currentCar->getCurrentLapTime());
	mvprintw(header_top + carno + 1,left_border + col_fastest_lap,"%-6u", currentCar->getFastestLapTime());
	
	// turn off personal best in green
	attroff(COLOR_PAIR(2));
	mvprintw(header_top + carno + 1,left_border + col_diff,"%-6u",(currentCar->getCurrentLapTime() - currentCar->getFastestLapTime()));
	mvprintw(header_top + carno + 1,left_border + col_laps,"%-5u",currentCar->getLaps());
	mvprintw(header_top + carno + 1,left_border + col_pits,"%-3u",currentCar->getPitStops());
	
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



