#pragma once

#include <string>

#include "CarStatus.h"
#include "pb/SessionMessage.pb.h"


class Session {

 public:

  enum session_type {TRAINING, QUALIFYING, RACE};
  enum session_status {RUNNING, PAUSED, STOPPED, FINISH, SAFETY_CAR};
  enum session_rule_type {TIME_BASED, LAP_BASED};
  enum session_pit_rule {NONE, MINIMUM_STOPS};


 private:

	SlotHub::SessionMessage *sm;

  
  // this script checks if the session is sane and also calculates the current
  // car positioning based on the session_type and rule_type
  std::string session_rule_script;
  
  // this script checks whether cars have followed the pit rule, if not
  // cars lap counting or timer might be paused until rule was obeyed
  std::string pit_rule_script;

  session_type type;
  session_status status;
  session_rule_type rule_type;
  session_pit_rule pit_rule;
  

  unsigned int time_elapsed;
  unsigned int time_set;

  unsigned int laps_elapsed;
  unsigned int laps_set;

  unsigned int start_time;

  std::map<int,CarStatus*> standings;


 public:

	Session();
	
	~Session();

  std::map<int,CarStatus*> getStandings();

  
  // updates the current standings based on the car status 
  // that was passed. Normally that would happen inside the
  // ControlUnit when a car finished a lap.
  void updateStandigs(CarStatus *status);

  void setSessionType(session_type type);
  session_type getSessionType();

  void setSessionStatus(session_status status);
  session_status getSessionStatus();

  void setSessionRuleType(session_rule_type rule_type);
  session_rule_type getSessionRuleType();
  

  void setPitRule(session_pit_rule rule);
  session_pit_rule getPitRule();


  void setSessionRuleScript(std::string *rule_script);
  std::string getSessionRuleScript();

  void setPitRuleScript(std::string *rule_script);
  std::string getPitRuleScript();

  
  unsigned int getTimeElapsed();
  void setTimeElapsed(unsigned int elapsed);

  unsigned int getTimeSet();
  void setTimeSet(unsigned int set);


  unsigned int getLapsElapsed();
  void setLapsElapsed(unsigned int elapsed);


  unsigned int getLapsSet();
  void setLapsSet(unsigned int set);


  unsigned int getStartTime();
  void setStartTime(unsigned int time);

};
