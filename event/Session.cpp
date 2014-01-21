#include "Session.h"


	Session::Session(){
		
		sm = new SlotHub::SessionMessage();
		
		
		}	
  
  
  
	Session::~Session(){
		
		delete (sm);
		
		
		}	
  	
  
  
  // updates the current standings based on the car status 
  // that was passed. Normally that would happen inside the
  // ControlUnit when a car finished a lap.
  void updateStandigs(CarStatus *status) {
	  
	  
	  
	  
	  
	}


 std::map<int,CarStatus*> Session::getStandings() { return standings;}
 

  void Session::setSessionType(Session::session_type type){ this->type = type;}
  
  Session::session_type Session::getSessionType(){return this->type;}


  void Session::setSessionStatus(session_status status){this->status = status;}
  Session::session_status Session::getSessionStatus() {  return this->status; }

  void Session::setSessionRuleType(session_rule_type rule_type){this->rule_type = rule_type;}
  Session::session_rule_type Session::getSessionRuleType(){return rule_type;}
  

  void Session::setPitRule(session_pit_rule rule){this->pit_rule = rule;}
  Session::session_pit_rule Session::getPitRule(){return this->pit_rule; }


  void Session::setSessionRuleScript(std::string *rule_script){ this->session_rule_script = std::string(rule_script->c_str());}
  std::string Session::getSessionRuleScript(){return std::string(session_rule_script);};

  void Session::setPitRuleScript(std::string *rule_script){this->pit_rule_script = std::string(rule_script->c_str());}
  std::string Session::getPitRuleScript(){return std::string(pit_rule_script);}

  
  unsigned int Session::getTimeElapsed(){return this->time_elapsed;}
  void Session::setTimeElapsed(unsigned int elapsed){this->time_elapsed = elapsed;}

  unsigned int Session::getTimeSet(){return this->time_set;}
  void Session::setTimeSet(unsigned int set){this->time_set = set;}


  unsigned int Session::getLapsElapsed(){return this->laps_elapsed;}
  void Session::setLapsElapsed(unsigned int elapsed){this->laps_elapsed = elapsed;}


  unsigned int Session::getLapsSet(){return this->laps_set;}
  void Session::setLapsSet(unsigned int set){this->laps_set = set;}


  unsigned int Session::getStartTime(){return this->start_time;}
  void Session::setStartTime(unsigned int time){this->start_time = time;}
