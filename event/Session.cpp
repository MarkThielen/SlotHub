#include "Session.h"

#include <iostream>

extern "C" {
	
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
	
}

	// ---------------------------------------
	// - Session Constructor
	// ---------------------------------------
	Session::Session(std::string sessionRuleName, std::string pitRuleName, unsigned int lapsSet, unsigned int timeSet){
		
		
		// initiate SessionMessage
		sm = new SlotHub::SessionMessage();
		
		// set session parameters
		setLapsSet(lapsSet);
		setTimeSet(timeSet);
		
		// intiate LUA
		L = luaL_newstate();		
		
		if (L == NULL)
			std::cout << "error creating new lua state" << std::endl;
		
		luaL_openlibs(L);
			
		// try to load session rule script
		if (luaL_dofile(L,sessionRuleName.c_str()) != 0)
			std::cout << "error loading " << sessionRuleName.c_str() << std::endl;
					
		}	
  
  
	// --------------------------------------------
	// - Destructor
	// --------------------------------------------
	Session::~Session(){
		
		// delete session message
		delete (sm);
		
		// close Lua 
		lua_close(L);	
			
		}	
  	
    
  // updates the current standings based on the car status 
  // that was passed. Normally that would happen inside the
  // ControlUnit when a car finished a lap.
  void Session::updateStandings(CarreraResponse cr ) {
	  
		  
		// -----------------------------------------------------------------------
		// - Calculate and show laptime of last car crossing the finish line 
		// -----------------------------------------------------------------------
		if (cr.getResponseType() == CarreraResponse::CAR_STATUS) {
		  
		  // find car in current map of cars
		  std::map<int,CarStatus*>::iterator iterCarStatus = standings.find(cr.getCarNumber()); 
		  
		  // update data for already active car
		  if(iterCarStatus != standings.end())
			iterCarStatus->second->updateTimeAndLapStatistics(cr.getTimer());
		  // ----------------------------------------------
		  // set initial data for new car on track
		  // ---------------------------------------------
		  else 
			// add new car to list of available cars
			standings.insert(std::pair<int, CarStatus*>(cr.getCarNumber(), new CarStatus(cr.getCarNumber())));
		  
		} 
		// ---------------------------------------------------------------
		// - collect fuel mode & status, start light status, tower mode 
		// ---------------------------------------------------------------
		else if (cr.getResponseType() == CarreraResponse::TRACK_STATUS) {
		  
		  for (std::map<int,CarStatus*>::iterator iterCarStatus = standings.begin(); iterCarStatus != standings.end(); ++iterCarStatus) {
		
			CarStatus *currentCar = iterCarStatus->second;
		
			// update fuel status
			currentCar->setFuelStatus(cr.getCarFuelStatus(currentCar->getCarNumber()));
		
			// update PitStop Statistics
			currentCar->updatePitStopStatistics(cr.carInPits(currentCar->getCarNumber()));
		
		  }
	}
	
	

	  
		
}


void Session::updatePositions() {
// -------------------------------------------------------------------------
// algorithm :
// - find car in standings
//		+ if not found, add it with maximum position number as it has not finished a lap and has not set any time. Postion no defined
// 		+ else 
//			+ updateTimeAndLapStatistics
//			+ determine new position based on set rules (laps / best time / etc.)
//			+ calculate new order. remove from current position, insert into new position, (potentially use vector instead of map or both)
// -------------------------------------------------------------------------
 
// ----------------------------------------------------------------------
// Call Lua Script with all available information to determine
// the state of
// - Ranking
// - etc.
//
// Session.LapsSet - Number of laps that were set for this session
// Session.LapsElapsed - Number of laps that have already been elapsed
// Session.TimeSet - Maximum time that the race should last
// Session.TimeElapsed - Time that has already elapsed
// Session.StartTime - Time when the Session has started
// Session.FastestLapTime - fastest lap time for this session
// 
// Table of
//		- KEY
//			- Car #
//		- ENTRY 			
//			- Fastest Lap Time
//			- Last Lap Time
//			- # of Laps
//			- # of Pit Stops
// 			- Position (?)
// -----------------------------------------------------------------------
// Results
// -----------------------------------------------------------------------
//
// - Current Ranking (CarPostion - Car#)
// - Session Status 
// - List of Cars with disabled Lap counting
//
// -----------------------------------------------------------------------	
	
	
	
	
	
}



	std::map<int,CarStatus*> Session::getStandings() { return standings;}
 

	void Session::setSessionType(Session::session_type type){ this->sm->set_type(type);}
  
	Session::session_type Session::getSessionType(){return session_type(this->sm->type());}


	  void Session::setSessionStatus(session_status status){this->sm->set_status(status);}
	  Session::session_status Session::getSessionStatus() {  return session_status(this->sm->status()); }

	  void Session::setSessionRuleType(session_rule_type rule_type){this->sm->set_rule_type(rule_type);}
	  Session::session_rule_type Session::getSessionRuleType(){return session_rule_type(this->sm->rule_type());}
	  

	  void Session::setPitRule(session_pit_rule rule){this->sm->set_pit_rule(rule);}
	  Session::session_pit_rule Session::getPitRule(){return session_pit_rule(this->sm->pit_rule()); }


	  void Session::setSessionRuleScript(std::string *rule_script){ this->session_rule_script = std::string(rule_script->c_str());}
	  std::string Session::getSessionRuleScript(){return std::string(session_rule_script);};

	  void Session::setPitRuleScript(std::string *rule_script){this->pit_rule_script = std::string(rule_script->c_str());}
	  std::string Session::getPitRuleScript(){return std::string(pit_rule_script);}

	  
	  unsigned int Session::getTimeElapsed(){return this->sm->time_elapsed();}
	  void Session::setTimeElapsed(unsigned int elapsed){this->sm->set_time_elapsed(elapsed);}

	  unsigned int Session::getTimeSet(){return this->sm->time_set();}
	  void Session::setTimeSet(unsigned int set){this->sm->set_time_set(set);}


	  unsigned int Session::getLapsElapsed(){return this->sm->laps_elapsed();}
	  void Session::setLapsElapsed(unsigned int elapsed){this->sm->set_laps_elapsed(elapsed);}


	  unsigned int Session::getLapsSet(){return this->sm->laps_set();}
	  void Session::setLapsSet(unsigned int set){this->sm->set_laps_set(set);}


	  unsigned int Session::getStartTime(){return this->sm->start_time();}
	  void Session::setStartTime(unsigned int time){this->sm->set_start_time(time);}
	  
	  
	  
	  unsigned int Session::getFastestLaptime() {return this->sm->fastest_laptime();}
	  void Session::setFastestLaptime(unsigned int laptime){this->sm->set_fastest_laptime(laptime);}
	  
