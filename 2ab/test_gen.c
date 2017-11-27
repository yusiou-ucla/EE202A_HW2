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

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

int running = 1;

void exit_handler(int s)
{
	printf("Exit requested \n");
  	running = 0;
}

int main(int argc, char *argv[]){


      //text parsing variables
      char str[20];
      double ts_gen[5] = {0.0}; //3 timestamp slots
      int i = 0; // iterator for ts slots
      int n = 0;
      int gen_channel = 2;
      int end = 0;
      //my code
      //int dev = init_qot(DEVICE, 1);
      //uint64_t nano_ts = qot_read_event_ts(&err);

      FILE * file;
      file = fopen( argv[1] , "r");

      if(file == NULL) {
      perror("Error opening file");
      return(-1);
      }

      int c;
      while(!end){
      //line by line

      i=0;
      //character by character
      do {
            c = fgetc(file);
            if(feof(file)) {
            end = 1;
            //printf("Breaking %d", i);
            break ;
            }
            if(c == '\n') {
            //printf("Newline %d", i);
            break ;
            }
            //printf("%c", c);
            str[i] = c;
            i++;
      } while(1);
            printf("string: %s\n", str);
            ts_gen[n] = atof(str);
            printf("%d, %1.9f\n", n, ts_gen[n]);
            n++;
      }

      int j;
      int ts_gen_s[n];
      long long ts_gen_ns[n];

      ts_gen_s[0] = 0;
      ts_gen_ns[0] = 0;

      for (j=1; j<n; j++){
            ts_gen_s[j] = (int) ts_gen[j];
            printf("Seconds: %d\n", ts_gen_s[j]);
            //printf("%1.9f\n", ts_gen[j] - (double) ts_gen_s[j]);
            printf("%9.9f\n", (ts_gen[j] - (double) ts_gen_s[j]) * 1000000000);
            ts_gen_ns[j] = (long long) (ts_gen[j] - (double) ts_gen_s[j]) * 1000000000;
            printf("Nanoseconds: %lld\n", ts_gen_ns[j]);
      }


      // if (init_qot("/dev/ptp1", gen_channel, ts_gen_s, ts_gen_ns, n)) { // index=2 corresponds to TIMER7
      //       printf("Initialize QoT failed\n");
      //       deinit_qot();
      //       exit(EXIT_FAILURE);
      // }

   	// while(fgets(str, 20, file)!=NULL){
    //      printf("Take %d\n", i);
   	// 	printf("string: %s", str);


    //      printf(feof(stdin))

   	// 	// ts_gen[i] = atof(str);
   	// 	// printf("%d, %1.9f\n", i, ts_gen[i]);
   	// 	i++;
   		
   //}
      
      // int err = 0;
      // uint64_t nano_ts;

   //double diff_1 = ts_gen[2] - ts_gen[0];
   //double diff_2 = ts_gen[4] - ts_gen[2];

      //init_qot(DEVICE,gen_channel);//,diff_1,diff_2);

      signal(SIGINT, exit_handler);
      signal(SIGTERM, exit_handler);

      while (running) 
      {
      usleep(10000);
      }
      
      if (deinit_qot()) {
            printf("Deinitialize QoT failed\n");
      }

      return(EXIT_SUCCESS);
}