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
#include <sys/epoll.h>

#include <linux/ptp_clock.h>

#include "event_ts.h"

#define DEVICE "/dev/ptp1"

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET 0x0100
#endif

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

#define MAXEVENTS 64

typedef struct {
  uint64_t *array;
  size_t used;
  size_t size;
} Array;

struct timespec ts;

static volatile int keepRunning = 1;

// void initArray(Array *a, size_t initialSize) {
//   a->array = (uint64_t *)malloc(initialSize * sizeof(uint64_t));
//   a->used = 0;
//   a->size = initialSize;
// }

// void insertArray(Array *a, uint64_t element) {
//   // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
//   // Therefore a->used can go up to a->size 
//   if (a->used == a->size) {
//     a->size *= 2;
//     a->array = (uint64_t *)realloc(a->array, a->size * sizeof(uint64_t));
//   }
//   a->array[a->used++] = element;
// }

// void freeArray(Array *a) {
//   free(a->array);
//   a->array = NULL;
//   a->used = a->size = 0;
// }

void intHandler(int dummy){
	// if (signo == SIGTERM || signo == SIGINT)
	// {
		keepRunning = 0;
	// }
}

int main(int argc, char *argv[]){

	signal(SIGINT, intHandler);

// //int status;
// system("testptp -d /dev/ptp1 -i 1 -L 1,1 >/dev/null 2>&1");
// system("testptp -d /dev/ptp1 -i 1 -e 100 >/dev/null 2>&1 &");
// system("PID=$!");
// printf("echo PTP flushing process launched at PID $PID.\n");
// system("sleep 1");
// system("kill -TERM $PID");
// printf("echo PTP queue flushed.\n");

    //signal(SIGTERM, intHandler);

	//Array events;
	int i = 0;
	int err = 0;
	uint64_t nano_ts;
	//uint64_t *test, *ptr;
	uint64_t events[150];
	//freeArray(&events);
	//initArray(&events, 1);  // initially 5 elements

	//double prev_ts;
	//int y;
	// for(y = 0; y < 5; y++){
	// 	printf("%"PRIu64"\n", events.array[y]);
	// }
	if (init_qot(DEVICE, 1)) {
	    printf("Initialize QoT time sync failed\n");
	    exit(EXIT_FAILURE);
	}

	// uint64_t check = 0;
	//int check = 0;


	while(keepRunning){
		//clock_gettime(CLOCK_REALTIME, &ts);
		//printf("System time: %ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
		
			nano_ts = qot_read_event_ts(&err);
			// printf("%"PRIu64"\n", nano_ts);
			//printf("%llu\n", nano_ts);
			if(nano_ts) {
				//printf("%d, Inserting into array\n", i);
				//insertArray(&events, nano_ts);
				events[i] = nano_ts;
				printf("%"PRIu64"\n", nano_ts);

				i++;
			}

			// printf("%"PRIu64"\n", nano_ts);
			// printf("%llu\n", nano_ts);
			
	}

	FILE * file;
	file = fopen( argv[1] , "w");
	int x = 0;
	//ptr = test;
	//prev_ts = 0.0;
	// int n;

	// for (n=0; n<i; n++){
	// 	fprintf(file, "event %d, %"PRIu64"\n", n, events[n]);
	// }
	// //fprintf(file, "0.000000000\n"); //first timestamp is always 0
	// while(x<i){

	// 	if ((events[1]-events[0])<1000000000){
	// 	//fprintf(file, "%9.9f\n", (double)((events.array[x] - events.array[0])/1000000000.0));
	// 	fprintf(file, "%9.9f\n", (double)((events[x] - events[0])/1000000000.0));
	// } else {
	// 	fprintf(file, "%9.9f\n", (double)((events[x] - events[1])/1000000000.0));
	// }
	// 	//prev_ts = (double)(events.array[x+1] - events.array[x])/1000000000.0;
	// 	//fprintf(file, "%"PRIu64"\n", *(ptr+ 1) - *(ptr));
	// 	//fprintf(file, "%"PRIu64"\n", events.array[x++]);  // print xth element
	// 	x++;
	// }

	double events_temp[i];
	int flushFlag;

	while(x<i){

		if ((events[1]-events[0])<1000000000){
		//fprintf(file, "%9.9f\n", (double)((events.array[x] - events.array[0])/1000000000.0));
		events_temp[x] = (double)((events[x] - events[0])/1000000000.0);
		flushFlag = 0;
		printf("events_temp: %9.9f\n", events_temp[x]);
		} else {
		events_temp[x] = (double)((events[x] - events[1])/1000000000.0);
		flushFlag = 1;
		printf("events_temp: %9.9f\n", events_temp[x]);
	}
	x++;
	}

	printf("number of events captured: %d\n", i);

	int j;

	for (j=flushFlag; j<i; j++){
		fprintf(file, "event number: %d, %9.9f\n", j, events_temp[j]);
	}
	//freeArray(&events);
	
	deinit_qot();

	// printf("%"PRIu64"\n", test[1]-test[0]);
	// double num = (double)(test[1]-test[0]);
	// num = num/1000000000.0;
	// printf("%9.9f\n", num);
	return 0;
}
