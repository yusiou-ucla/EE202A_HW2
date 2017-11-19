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

int init_qot(char* dev, int channel, int ts_gen_s, long long ts_gen_ns, long long sec, long long nsec, long long pnsec, int fd_m);//, double diff1, double diff2);
int deinit_qot(int fd_m);
//uint64_t qot_read_event_ts(int* err);

#endif
