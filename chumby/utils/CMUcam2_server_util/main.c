#include <stdlib.h>
#include <stdio.h>
#include "udp_server.h"
#include "cmucam2.h"

#define UDP_PORT 3555

int main () {
#if 0
    unsigned char buf [1024];

    udp_server_init(UDP_PORT);
    udp_receive (buf, sizeof(buf));
    udp_send (buf, sizeof(buf));
#endif

    cmucam2_init();
    cmucam2_dump_raw();
    return 0;
}
