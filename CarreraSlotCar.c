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

#include "carrera.h"

char *portname = "/dev/ttyUSB0";
char *CARRERA_TIMING_QUERY = "\"?";
char *CARRERA_CU_FIRMWARE = "\"0";

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf        ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf        ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf        ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf        ("error %d setting term attributes", errno);
}


int get4Bits(char c) { return (int)(c & 0xf);}

/* unsigned int getTimer (struct STRUCT_CARRERA_LAPINFO *scl ) { */

/*   return  */
/*     (get4Bits(scl->timer[1]) << 28) + */
/*     (get4Bits(scl->timer[0]) << 24) + */
/*     (get4Bits(scl->timer[3]) << 20) + */
/*     (get4Bits(scl->timer[2]) << 16) + */
/*     (get4Bits(scl->timer[5]) << 12) + */
/*     (get4Bits(scl->timer[4]) << 8) + */
/*     (get4Bits(scl->timer[7]) << 4) + */
/*     (get4Bits(scl->timer[6])); */

/* } */


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


// -------------------------------------------------------------
//
// -------------------------------------------------------------




void main (int args, char* argv[]){

 struct STRUCT_CARRERA_RESPONSE *carrera_response = 
   (struct STRUCT_CARRERA_RESPONSE *)malloc(sizeof (struct STRUCT_CARRERA_RESPONSE));


 
int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);


printf("hello world ! args: %i \n",args);


if (fd < 0)
{
        printf        ("error %d opening %s: %s", errno, portname, strerror (errno));
        return;
}

set_interface_attribs (fd, B19200 , 0);  // set speed to 115,200 bps, 8n1 (no parity)
set_blocking (fd, 0);                // set no blocking

//char buf [20]
// get CU Firmware Version

 /* memset(buf,0,20); */
 /* write (fd, CARRERA_CU_FIRMWARE, strlen(CARRERA_CU_FIRMWARE));  */
 /*   read (fd, buf, sizeof buf); */
 /*   printf("CU Firmware # %s\n",buf); */

 unsigned int prev_timer=0;

 while (1){


   unsigned int timer = 0, laptime = 0;

   write (fd, CARRERA_TIMING_QUERY, strlen(CARRERA_TIMING_QUERY));           // send 7 character greeting

   memset(carrera_response,0,sizeof(struct STRUCT_CARRERA_RESPONSE));

   int n = read (fd, carrera_response, sizeof (struct STRUCT_CARRERA_RESPONSE) + 1);

   //printf("Buffer : %s : %i %c \n",carrera_response,n, carrera_response->car_number );



   // -----------------------------------------------------------------------
   // - Calculate and show laptime of last car crossing the finish line 
   // -----------------------------------------------------------------------
   if (get4Bits(carrera_response->car_number) >= CARRERA_MIN_CAR_NUMBER 
       && get4Bits(carrera_response->car_number) <= CARRERA_MAX_CAR_NUMBER) {

     timer = getTimer (((union DATA)carrera_response->data).scl);

     // -----------------------------------------------------
     // previous timer should be set after one lap, if yes
     // we are able to calculate a meaningful lap time
     // -----------------------------------------------------
     if(prev_timer > 0) {
       
       // laptime is substracting the current timer with the previous
       // timer
       laptime = timer - prev_timer;
       
       // show Car number, current timer and laptime
       printf ("Car #%c T(I): %u LT = %u \n", 
	  carrera_response->car_number, timer,laptime);
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

     printf("Track Status : (Fuel Mode)%i Fuel Status (1)%i (2)%i (3)%i (4)%i (5)%i (6)%i (7)%i (8)%i (FuelBM)%i%i (SLS)%i\n",
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_mode),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[0]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[1]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[2]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[3]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[4]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[5]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[6]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[7]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[0]),
	    get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[1]),
	    get4Bits(((union DATA)carrera_response->data).scts.start_light_status)


	    );

   }




 usleep (50000);

 }





} 






