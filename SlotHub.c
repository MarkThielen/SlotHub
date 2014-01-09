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

#include <iostream>

#include <map>

#include <argtable2.h>

#include <thread>


#include "ttyTools.h"
#include "CarStatus.h"
#include "TrackStatus.h"
#include "CarreraResponse.h"

#include "ControlUnit.h"
#include "TextDisplay.h"

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
  
  struct arg_lit *startControlUnit = arg_lit0("c","ControlUnit","start the ControlUnit");
  struct arg_lit *startTextDisplay = arg_lit0("t","TimeDisplay","start the Display");
  
  struct arg_str *bindAddress = arg_str0("b","bind","protocol://address:port","address/port to bind (ControlUnit)");
  struct arg_str *connectTo = arg_str0("s","server","protocol://address:port","address/port to connect to (TimeDisplay)");


  struct arg_end *end = arg_end(20);

  

  void *argtable[] = { 
    startControlUnit, startTextDisplay, ttyDevice,bindAddress, connectTo, end      
  };

  // check argtable
  if (arg_nullcheck(argtable) != 0)
    printf ("error: insufficient memory\n");

  // set default value for tty device to use
  // will be overwritten if something was passed
  // on the cmd line
  ttyDevice->filename[0] = "/dev/ttyUSB0";
  
  // set default value for control unit 
  // server to bind to
  bindAddress->sval[0] = "tcp://*:8190";

  // set default value for Time Display 
  // client to connect to
  connectTo->sval[0] = "tcp://localhost:8190";


  // parse argtable
  int parse_errors = arg_parse(argc,argv,argtable);

  if(parse_errors > 0)
    arg_print_errors(stdout,end,"SlotHub");

  // --------------------------------------------
  // - Create TextDisplay Object and start thread
  // --------------------------------------------
  if (startTextDisplay->count > 0) {

    TextDisplay *td = new TextDisplay(connectTo->sval[0]);

    std::thread tdt = td->start();
    std::cout << "TextDisplay started" << std::endl;
    tdt.join();

    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

    return 0;
  }

  // -------------------------------------------
  // - create ControlUnit Object and start thread
  // -------------------------------------------
  else if(startControlUnit->count > 0) {
   
    
    ControlUnit *cu = new ControlUnit(ttyDevice->filename[0],bindAddress->sval[0]);
  
    std::thread cut = cu->start();
    std::cout << "ControlUnit started" << " (" << cu->getBindAddress()  << ")" << std::endl;
    cut.join();

    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    
    return 0;

  }
  // --------------------------------------------------------
  // - start control unit and display in one process
  // --------------------------------------------------------
  else {

    TextDisplay *td = new TextDisplay("ipc://SlotHub.ipc");
    std::thread tdt = td->start();
    
    
    ControlUnit *cu = new ControlUnit(ttyDevice->filename[0],bindAddress->sval[0]);
    std::thread cut = cu->start();
    
    tdt.join();

    // set execution flag to stop
    cu->stop();

    // wait for thread to terminate
    cut.join();
    

    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));    
    return 0;
  }
  



   arg_print_glossary(stdout,argtable," %-40s %s\n");

   arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

   return 0;

} 






