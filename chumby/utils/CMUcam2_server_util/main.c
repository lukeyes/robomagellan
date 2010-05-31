#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "udp_server.h"
#include "cmucam2.h"

#define UDP_PORT 3555
#define NUM_PIXEL_ROWS 239
#define NUM_PIXEL_COLS 159

int main () {
    unsigned char * buf, *p;
    int size = 150000;
    int i=0;

    udp_server_init(UDP_PORT);
    cmucam2_init();

    if(!(buf = malloc(size))) {
        fprintf(stderr, "main: malloc failed\n");
    }

    // Wait for client image dump request
    udp_receive (buf, size);

    memset(buf, 0, size);
    // get image dump
    cmucam2_dump_raw(buf, size);
    while(buf[i++]);
    fprintf(stderr, "\nnum elements is: %d\n", --i);

    for(i=0, p=buf; i<NUM_PIXEL_ROWS; ++i, p+=NUM_PIXEL_COLS*3) {
        fprintf(stderr, "[%d]", i+1);
        // Send image data to client
        udp_send (p, NUM_PIXEL_COLS*3);
        usleep(100000);
    }

    free(buf);
    return 0;
}
