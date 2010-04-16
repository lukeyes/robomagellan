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

// Must line up w/ STATES in main.h
const char * state_strings[NUM_STATES] = {
    "INIT_STATE",
    "NAVIGATION_STATE",
    "OBJECT_AVOIDANCE_STATE",
    "TRACK_STATE",
    "DONE_STATE",
    "ERROR_STATE"
};

void exit_routine (int sig);

// char * rgp format is "rrr,ggg,bbb"were each letter is a base 10 digit
void set_lcd_color(char * rgb) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "imgtool --mode=draw --fill=%s --fb=0 > /dev/null 2>&1", rgb);
    system(cmd);
}
void write_lcd(char * text, int row, int col) {
    char cmd[64];
    //snprintf(cmd, sizeof(cmd), "fbwrite --pos=%d,%d \"%s\n\" 2> /dev/null", col,row,text);
    snprintf(cmd, sizeof(cmd), "fbwriteln %d %s", row,text);
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

    if(debug)
        //Disable comm line echo
        system("stty -echo");
        
    // Begin with init state
    next_state = INIT_STATE;

    // Initially no errors
    snprintf(state_data.error_str, sizeof(state_data.error_str), "NO_ERROR");

    while (1) {
        current_state = next_state;
        strcpy(state_data.current_state_str, STATE_STR(current_state));
        switch (current_state) {
            case INIT_STATE:
                if(debug) {
                    // Set LCD to white 
                    set_lcd_color("255,255,255");
                    write_lcd("INIT_STATE", 0, 0);
                }
                init_state();
                break;
            case NAVIGATION_STATE:
                if(debug) {
                    // Set LCD to light green
                    set_lcd_color("124,252,0");
                    write_lcd("NAVIGATION_STATE", 0, 0);
                }
                navigation_state();
                break;
            case OBJECT_AVOIDANCE_STATE:
                if(debug) {
                    // Set LCD to light blue
                    set_lcd_color("0,191,255");
                    write_lcd("OBJECT_AVOIDANCE_STATE", 0, 0);
                }
                object_avoidance_state();
                break;
            case TRACK_STATE:
                if (debug) {
                    // Set LCD to orange-red 
                    set_lcd_color("255,69,0");
                    write_lcd("TRACK_STATE", 0, 0);
                }
                track_state();
                break;
            case DONE_STATE:
                if (debug) {
                    // Set LCD to purple
                    set_lcd_color("160,32,240");
                    write_lcd("DONE_STATE", 0, 0);
                }
                done_state();
                break;
            case ERROR_STATE:
                if (debug) {
                    // Set LCD to firebrick red
                    set_lcd_color("178,34,34");
                    write_lcd("ERROR_STATE", 0, 0);
                    write_lcd(state_data.error_str, 1, 0);
                    write_lcd("Please check device connection and reboot", 2, 0);
                }
                while(1)
                    sleep(1);
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

        console_gotoxy(0, 15);
        fprintf(stderr, "Stopping Car...");
        console_gotoxy(0, 16);
        fprintf(stderr, "Exiting");
        console_gotoxy(0, 17);
        // Set LCD to gray 
        set_lcd_color("105,105,105");
        write_lcd("You Have Terminated Program Execution", 0, 0);
    }
    exit(0);
}
