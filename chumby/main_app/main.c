/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  main.c
 *  This is the main state machine control module for the robomagellan
 *  project.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "main.h"
#include "done_state.h"
#include "track_state.h"
#include "object_avoidance_state.h"
#include "navigation_state.h"
#include "init_state.h"

void exit_routine (int sig);

int main (int argc, char **argv) {
    int current_state;
    debug = 0;

    // Enable Control-C detection
    signal(SIGINT, exit_routine);

    // Check for debug option
    if(getopt(argc, argv, "d:") != -1)
        debug = 1;

    // Begin with init state
    next_state = INIT_STATE;

    while (1) {
        current_state = next_state;
        switch (current_state) {
            case INIT_STATE:
                init_state();
                break;
            case NAVIGATION_STATE:
                navigation_state();
                break;
            case OBJECT_AVOIDANCE_STATE:
                object_avoidance_state();
                break;
            case TRACK_STATE:
                track_state();
                break;
            case DONE_STATE:
                done_state();
                break;
        }
        fprintf(stderr, "main\n\n");
        sleep(1);
    }
}

void exit_routine (int sig) {
    // To stop car
    done_state();
    fprintf(stderr, "exiting program\n");
    exit(0);
}
