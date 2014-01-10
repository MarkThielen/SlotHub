#pragma once

#include <string>

class Session {

 public:

  enum session_type {TRAINING, QUALIFYING, RACE};
  enum session_status {RUNNING, PAUSED, STOPPED, FINISH, SAFETY_CAR};
  enum session_rule_type {TIME_BASED, LAP_BASED};
  enum pit_rules {NONE, MINIMUM_STOPS};


 private:

  std::string session_rule_script;
  std::string pit_rule_script;


  unsigned int time_elapsed;
  unsigned int time_set;

  unsigned int laps_elapsed;
  unsigned int laps_set;

  unsigned int start_time;


 public:

  void setSessionType(session_type type);
  session_type getSessionType();

  void setSessionStatus(session_status status);
  session_status getSessionStatus();

  void setSessionRuleType(session_rule_type rule_type);
  session_rule_type getSessionRuleType();
  

  void setPitRule(pit_rules rule);
  pit_rules getPitRule();


  void setSessionRuleScript(std::string *rule_script);
  std::string getSessionRuleScript();

  void setPitRuleScript(std::string *rule_script);
  std::string getPitRuleScript();

  
  unsigned int getTimeElapsed();
  void setTimeElapsed(unsigned int elapsed);

  unsigned int getTimeSet();
  void setTimeSet(unsinged int set);


  unsigned int getLapsElapsed();
  void setLapsElapsed(unsigned int elapsed);


  unsigned int getLapsSet();
  void setLapsSet(unsigned int set);


  unsigned int getStartTime();
  void setStartTime(unsigned int time);

};
