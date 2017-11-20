#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include <linux/ptp_clock.h>

#include "event_gen.h"

#define DEVICE "/dev/ptp1"

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET 0x0100
#endif

#define FD_TO_CLOCKID(fd) ((~(clockid_t) (fd) << 3) | 3)
#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

int running = 1;
clockid_t clkid_m; 	    /* Clock ids */
struct timespec ts;				/* Time storage */
int fd_m;                   /* device file descriptor */
struct timespec tswait;
struct timespec rsttime;

void exit_handler(int s)
{
	printf("Exit requested \n");
  	running = 0;
}

int main(int argc, char *argv[]){

	    /* Open the character device */
    fd_m = open("/dev/ptp1", O_RDWR);
    if (fd_m < 0) {
        fprintf(stderr, "opening device %s: %s\n",
                "/dev/ptp1", strerror(errno));
        return -1;
    }
    printf("Device opened %d\n", fd_m);

	clkid_m = FD_TO_CLOCKID(fd_m);
    if (CLOCK_INVALID == clkid_m) {
	printf("failed to read clock id for %d\n", fd_m);
	return -1;
    }
    printf("clock id is %lld\n", clkid_m);

    rsttime.tv_sec = 0;
    rsttime.tv_nsec = 0;//500000000;
    int status = clock_settime(clkid_m, &rsttime);
    /* Get the clock current time */
    if (clock_gettime(clkid_m, &ts)) {
	printf("clock_gettime failed for %d\n",fd_m);
	return -1;
    }

      //text parsing variables
      char str[20];
      //double ts_gen[5] = {0.0}; //3 timestamp slots
      int i = 0; // iterator for ts slots
      //int n = 0;
      int gen_channel = 2;
      //int end = 0;
      //my code
      //int dev = init_qot(DEVICE, 1);
      //uint64_t nano_ts = qot_read_event_ts(&err);

      FILE * file;
      file = fopen( argv[1] , "r");

      if(file == NULL) {
      perror("Error opening file");
      return(-1);
      }

      double ts_gen[150];

      while (fgets(str, 20, file)){
      		ts_gen[i] = atof(str);
      		//printf("string: %s", str);
          //printf("conversion: %lf\n", atof(str));
      		//printf("double: %f\n", ts_gen[i]);
      		i++;
      }
      // int y;
      // for(y = 0; y < i; y++){
      //   printf("double: %f\n", ts_gen[y]);
      // }
      
      printf("Number of events to be requested: %d\n", i);
      int j;
      int ts_gen_s[i];
      long long ts_gen_ns[i];


      // while (running) 
      // {

      for (j=0; j<i; j++){
            ts_gen_s[j] = ts_gen[j];
            //printf("Doublevalue: %9.9f\n", ts_gen[j]);
            //printf("Seconds: %2.2d\n", ts_gen_s[j]);
            //printf("%1.9f\n", ts_gen[j] - (double) ts_gen_s[j]);
            //printf("%9.9f\n", (ts_gen[j] - (double) ts_gen_s[j])* 1000000000);
            ts_gen_ns[j] = (long long) ((ts_gen[j] - (double) ts_gen_s[j]) * 1000000000);
            //printf("Nanoseconds: %lld\n", ts_gen_ns[j]);
            long long perOut = 90000000;
            long long perWait = 80000000;

          if (init_qot("/dev/ptp1", gen_channel, ts_gen_s[j], ts_gen_ns[j], ts.tv_sec, ts.tv_nsec, perOut, fd_m)) { // index=2 corresponds to TIMER7
            printf("Initialize QoT failed\n");
            deinit_qot(fd_m);
            exit(EXIT_FAILURE);

          }
          //busywait

            uint64_t pulse_time = ((long long)(ts.tv_sec + ts_gen_s[j] + 2) * 1000000000) + ts.tv_nsec + ts_gen_ns[j] + perWait;
            //printf("ts_gen_s: %d\n", ts_gen_s[j]);
            //printf("Pulse time: %" PRIu64 "\n", pulse_time);

            uint64_t wait_time;
          do{
              if (clock_gettime(clkid_m, &tswait)) {
                printf("clock_gettime failed for %d\n",fd_m);
                return -1;
              }
              wait_time = ((long long)tswait.tv_sec * 1000000000) + (long long) tswait.tv_nsec;
              //printf("Wait time: %" PRIu64 "\n", wait_time);

          } while( wait_time < pulse_time);
            //printf("Wait time:  %" PRIu64 "\n", wait_time);
            if (init_qot("/dev/ptp1", gen_channel, ts_gen_s[j], ts_gen_ns[j] + perWait, ts.tv_sec, ts.tv_nsec, 0, fd_m)) { // index=2 corresponds to TIMER7
            printf("Initialize QoT failed\n");
            deinit_qot(fd_m);
            exit(EXIT_FAILURE);

          }

      // }


      signal(SIGINT, exit_handler);
      signal(SIGTERM, exit_handler);

      }
      
      if (deinit_qot(fd_m)) {
            printf("Deinitialize QoT failed\n");
      }

      return(EXIT_SUCCESS);
}