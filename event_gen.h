/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on helloworld_capture.c, a simple C example showing how to register
 * to listen for capture events by Fatima Anwar and Andrew Symington.
 */

#ifndef EVENT_GEN_H
#define EVENT_GEN_H

#include <stdint.h>

int init_event_gen(char* dev, int channel);//, double diff1, double diff2);
int deinit_event_gen();
//uint64_t qot_read_event_ts(int* err);

#endif
