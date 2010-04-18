#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "usb_i2c.h"
#include "sonar.h"
#include "drive_control.h"

#define THRESH 190  //about 4.8 meters
#define SPEED_FAST 100
#define SPEED_SLOW 80

int tty;
int debug = 0;

void exit_routine (int sig);
void avoid_object();

static int speed_fast = SPEED_FAST;
static int speed_slow = SPEED_SLOW;

int main(int argc, char * argv[])
{
    
    /*Initialize all variables*/
    int front_sonar;
    int right_sonar;
    char c = 0;

    // Enable Control-C detection
    signal(SIGINT, exit_routine);

    if((c = getopt(argc, argv, "s:")) != -1) {
        switch(c) {
            case 's':
                if(!optarg)
                    exit(0);
                sscanf(optarg, "%d,%d", &speed_fast, &speed_slow);
                break;
            case '?':
                exit(0);
                break;
        }
    }

    tty =  initialize_i2c();
    initialize_sonar(tty);
    init_car();

    do {
        fprintf(stderr,"enter 'g' to start the car: ",front_sonar);
        c = (char)getc(stdin);
    } while (c != 'g');

    drive_forward(speed_fast);

    while(1) {
        take_range( tty );
        front_sonar = get_front_range(tty);
        if(debug)
            fprintf(stderr,"[fs: %d] ",front_sonar);
        if (front_sonar < THRESH) {
            avoid_object();
            if(debug)
                fprintf(stderr,"\nAfter avoid, front sonar: %d\nright sonar: %d",front_sonar,get_right_range(tty));
            stop_car();
        }
    }
    close(tty);
    tty = 0;
    return 0;
}

void avoid_object()
{
    int front_sonar;
    int right_sonar;
    take_range(tty);
    front_sonar = get_front_range(tty);
    right_sonar = get_right_range(tty);
    drive_forward(speed_slow);
    while(front_sonar < THRESH || right_sonar <  THRESH) {
        if(front_sonar < THRESH) {
            if(front_sonar > 95) //do 20% turn
                turn_left(20);
            else if(front_sonar > 60) //do 60% turn
                turn_left(60);
            else if(front_sonar > 25)
                turn_left(100); //do max turn
            else
                stop_car();
        }
        else
            turn_center();

        take_range(tty);
        front_sonar = get_front_range(tty);
        right_sonar = get_right_range(tty);

        if(debug) {
            fprintf(stderr,"[fs: %d]",front_sonar);
            fprintf(stderr,"[rs: %d]",right_sonar);
        }
    }
}

void exit_routine (int sig) {
    turn_center();
    stop_car();
    fprintf(stderr, "stopped car\n");
    exit(0);
}
