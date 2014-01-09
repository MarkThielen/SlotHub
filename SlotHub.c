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
  struct arg_lit *startTextDisplay = arg_lit0("d","Display","start the Display");

  struct arg_end *end = arg_end(20);

  

  void *argtable[] = { 
    startControlUnit, startTextDisplay, ttyDevice, end      
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

  // Create TextDisplay Object and start it
  
  if (startTextDisplay->count > 0) {
    TextDisplay *td = new TextDisplay();
    std::thread tdt = td->start();
    std::cout << "TextDisplay started" << std::endl;
    tdt.join();
    
  }


  if(startControlUnit->count > 0) {
  // create a ControlUnit Object
    ControlUnit *cu = new ControlUnit(ttyDevice->filename[0]);
  
    std::thread cut = cu->start();
    
    std::cout << "ControlUnit started" << std::endl;

    cut.join();

  }

 return 0;

} 






