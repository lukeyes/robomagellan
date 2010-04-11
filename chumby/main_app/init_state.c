//init all peripherals
//verify all peripherals
//output status to screen
//receive gps coordinates
#include <stdio.h>
#include "main.h"
#include "init_state.h"
#include "sonar.h"
#include "camera.h"
#include "usb_i2c.h"
#include "gps.h"
void init_state() {
}

int init_gps() {
        int tty = initialize_gps();
        if(tty < 0){
                if(debug)
                        fprintf(stderr,"Problem initializing GPS\n");
                return 0;
        }

        else {
                if(debug)
                        fprintf(stderr,"GPS initialization successful");
                return tty;
        }
}

int init_i2c() {
        int tty = initialize_i2c();
        if(tty < 0){
                if(debug)
                        fprintf(stderr,"Problem initializing i2c\n");
                return 0;       
        }

        else {
                if(debug)
                        fprintf(stderr,"i2c initialization successful");
                return tty;
        }
}

int init_camera()
{
        int tty = initialize_camera();
        if(tty < 0) {
                if(debug)
                        fprintf(stderr,"Problem initializing camera\n");
                return 0;       
        }

        else {
                if(debug)
                        fprintf(stderr,"camera initialization successful");
                return tty;
        }

}
int init_sonar(int tty)
{
        int retval = initialize_sonar(tty);
        if(debug) {
                if(retval==0)
                        fprintf(stderr,"sonar initialization successful");
        }
}

int init_compass(int tty)
{
	return 0;
}
