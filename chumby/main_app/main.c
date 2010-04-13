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
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "main.h"
#include "done_state.h"
#include "track_state.h"
#include "object_avoidance_state.h"
#include "navigation_state.h"
#include "init_state.h"
#include "console.h"

void exit_routine (int sig);

// char * rgp format is "rrr,ggg,bbb"were each letter is a base 10 digit
void set_lcd_color(char * rgb) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "imgtool --mode=draw --fill=%s --fb=0 > /dev/null 2>&1", rgb);
    system(cmd);
}
void write_lcd(char * text) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "fbwrite \"%s\"", text);
    system(cmd);
}

int main (int argc, char **argv) {
    int current_state;
    debug = 0;

    // Enable Control-C detection
    signal(SIGINT, exit_routine);

    // Check for debug option
    if(getopt(argc, argv, "d") != -1)
        debug = 1;

    // Begin with init state
    next_state = INIT_STATE;

    while (1) {
        current_state = next_state;
        switch (current_state) {
            case INIT_STATE:
                strcpy(state_data.current_state_str, "INIT_STATE");
                if(debug) {
                    // Set LCD to white 
                    set_lcd_color("255,255,255");
                    write_lcd("INIT_STATE");
                }
                init_state();
                break;
            case NAVIGATION_STATE:
                strcpy(state_data.current_state_str, "NAVIGATION_STATE");
                if(debug) {
                    // Set LCD to light green
                    set_lcd_color("124,252,0");
                    write_lcd("NAVIGATION_STATE");
                }
                navigation_state();
                break;
            case OBJECT_AVOIDANCE_STATE:
                strcpy(state_data.current_state_str, "OBJECT_AVOIDANCE_STATE");
                if(debug) {
                    // Set LCD to light blue
                    set_lcd_color("0,191,255");
                    write_lcd("OBJECT_AVOIDANCE_STATE");
                }
                object_avoidance_state();
                break;
            case TRACK_STATE:
                strcpy(state_data.current_state_str, "TRACK_STATE");
                if (debug) {
                    // Set LCD to orange-red 
                    set_lcd_color("255,69,0");
                    write_lcd("TRACK_STATE");
                }
                track_state();
                break;
            case DONE_STATE:
                strcpy(state_data.current_state_str, "DONE_STATE");
                if (debug) {
                    // Set LCD to purple
                    set_lcd_color("160,32,240");
                    write_lcd("DONE_STATE");
                }
                done_state();
                break;
        }
        sleep(2);
    }
}

void exit_routine (int sig) {
    // Go to done state to stop car
    done_state();
    strcpy(state_data.current_state_str, "DONE_STATE");

    if (debug) {
        // Enable comm line echo
        system("stty echo");

        console_gotoxy(0, 14);
        fprintf(stderr, "Stopping Car...");
        console_gotoxy(0, 15);
        fprintf(stderr, "Exiting");
        console_gotoxy(0, 16);
        // Set LCD to gray 
        set_lcd_color("105,105,105");
        write_lcd("You Have Terminated Program Execution");
    }
    exit(0);
}
