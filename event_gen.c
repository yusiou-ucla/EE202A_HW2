#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/ptp_clock.h>

#define PTP_PIN_SETFUNC    _IOW(PTP_CLK_MAGIC, 7, struct ptp_pin_desc)



struct ptp_pin_desc {
    /*
     * Hardware specific human readable pin name. This field is
     * set by the kernel during the PTP_PIN_GETFUNC ioctl and is
     * ignored for the PTP_PIN_SETFUNC ioctl.
     */
    char name[64];
    /*
     * Pin index in the range of zero to ptp_clock_caps.n_pins - 1.
     */
    unsigned int index;
    /*
     * Which of the PTP_PF_xxx functions to use on this pin.
     */
    unsigned int func;
    /*
     * The specific channel to use for this function.
     * This corresponds to the 'index' field of the
     * PTP_EXTTS_REQUEST and PTP_PEROUT_REQUEST ioctls.
     */
    unsigned int chan;
    /*
     * Reserved for future use.
     */
    unsigned int rsv[5];
};

char *device_m;             /* PTP device */
int index_m;                /* Channel index, '1' corresponds to 'TIMER6' */

struct ptp_pin_desc desc;   /* Pin configuration */

struct timespec t1;
struct timespec t2;

// static int running = 1;

// static void exit_handler(int s)
// {
// 	printf("Exit requested \n");
//   	running = 0;
// }

/**
 * Initialize the QoT system for capturing input event timestamps.
 *
 * Params:
 * dev - PTP device file
 * channel - Channel index, '1' corresponds to 'TIMER6'
 */
int init_qot(char* dev, int channel, int ts_gen_s, long long ts_gen_ns, long long sec, long long nsec, long long pnsec, int fd_m)
{
    struct ptp_perout_request perout_request;	/* Programmable interrupt */
    // int n;

    // t1.tv_sec = 1;
    // t1.tv_nsec = 0;//500000000;

    // t2.tv_sec = 0;
    // t2.tv_nsec = nsec;

    device_m = dev;                             /* PTP device */
    index_m = channel;                          /* Channel index */

    // for(n=0; n<1; n++){
    // /* Open the character device */
    // fd_m = open(device_m, O_RDWR);
    // if (fd_m < 0) {
    //     fprintf(stderr, "opening device %s: %s\n",
    //             device_m, strerror(errno));
    //     return -1;
    // }
    // printf("Device opened %d\n", fd_m);

    memset(&desc, 0, sizeof(desc));
    desc.index = index_m;
    desc.func = 2;              // '2' corresponds to periodic output
    desc.chan = index_m;
    if (ioctl(fd_m, PTP_PIN_SETFUNC, &desc)) {
        printf("Set pin func failed for %d\n", fd_m);
        return -1;
    }
    //printf("Set pin func successful for %d\n", fd_m);

    
    //printf("QoT current time: %lld.%09u\n", sec, nsec);

    /* Configure pulse per second to start at deterministic point in future */
    

    memset(&perout_request, 0, sizeof(perout_request));
    int offset = 2;
    
        //pulseStart = ((long long)(nano_ts)) + (offset + ts_gen[n]) * 1000000000;

        perout_request.index = index_m;
        perout_request.start.sec = (long long)(sec) + offset + ts_gen_s;
        perout_request.start.nsec = (long long)nsec + ts_gen_ns;
        perout_request.period.sec = 0;
        perout_request.period.nsec = pnsec; // 100 msecond period, change it to desired value

        if (ioctl(fd_m, PTP_PEROUT_REQUEST, &perout_request)) {
            printf("cannot request periodic output for %d\n", fd_m);
            return -1;
        }

        printf("Output for requested time successful, outputting at %d.%09u: \n", offset + ts_gen_s, ts_gen_ns);

    return 0;
}

int deinit_qot(int fd_m)
{
    int ret = 0;

    /* Disable the pin */
    memset(&desc, 0, sizeof(desc));
    desc.index = index_m;
    desc.func = 0;              // '0' corresponds to no function
    desc.chan = index_m;
    if (ioctl(fd_m, PTP_PIN_SETFUNC, &desc)) {
        printf("Disable pin func failed for %d\n", fd_m);
        ret = -1;
    }

    /* Close the character device */
    close(fd_m);

    return ret;
}

// int main(void)
// {
//     // Setup QoT
//     if (init_qot("/dev/ptp1", 2)) { // index=2 corresponds to TIMER7
//         printf("Initialize QoT failed\n");
// 	deinit_qot();
//         exit(EXIT_FAILURE);
//     }

//     int err = 0;
//     uint64_t nano_ts;

//     signal(SIGINT, exit_handler);
//     while (running) 
//     {
// 	usleep(10000);
//     }


//     // Teardown QoT
//     if (deinit_qot()) {
//         printf("Deinitialize QoT failed\n");
//     }

//     return(EXIT_SUCCESS);
// }
