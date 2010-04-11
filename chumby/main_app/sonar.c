/*      A module that has functions that will read from the
 *      specified sonar sensor, the distance from that that
 *      sensor to the nearest object in inches.
 *
 *  Author:     Michael Ortiz (mtortiz.mail@gmail.com)
 *  Modified:   Scott Hollwedel (s.hollwedel@gmail.com)(1/22/10)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "usb_i2c.h"

#define SONAR_FIRST_ADDR 0xE0
#define SONAR_LAST_ADDR 0xE2
#define RANGE 140
#define GAIN 0

static int tty;

int sonar_init()
{
        tty = initialize_i2c();
        int address = SONAR_FIRST_ADDR; //Address of 1st sensor}
        int output = 0;

//Make sure everything works, in this case read the software version #
        while ( address <= SONAR_LAST_ADDR )
        {
                write_i2c(tty, address + 1, 0, 1, 0);
                read(tty,&output,1);
                address = address + 2;
                if(output != 10)
                {
                        fprintf(stderr, "Sonar is broken.\n");
                        return -1;
                }
        }

        // Set new range(6m)
        address = SONAR_FIRST_ADDR;
        while ( address <= SONAR_LAST_ADDR )
        {
                write_i2c(tty, address, 2, 1,RANGE);
                read(tty,&output,1);
                address= address + 2;
        }

        // Set new gain corresponding to range
        address = SONAR_FIRST_ADDR;
        while ( address <= SONAR_LAST_ADDR )
        {
                write_i2c(tty, address, 1, 1,GAIN);
                read(tty,&output,1);
                address = address + 2;
        }
        
        return tty;
}

void sonar_take_range()
{
        int address = SONAR_FIRST_ADDR; //Address of 1st sensor
        int output = 0;
        // Take range
        while ( address <= SONAR_LAST_ADDR )
        {
                write_i2c(tty, address, 0, 1,80);
                read(tty, &output, 1);
                address = address + 2;
        }
        usleep(70000); //Delay for ranging to complete 
}

int sonar_get_left()
{
        int output = 0;
        write_i2c(tty, 0xE1, 3, 1, 0);
        read(tty,&output,1);
        return output;
}

int sonar_get_front()
{
        int output = 0;
        write_i2c(tty, 0xE3, 3, 1, 0);
        read(tty, &output,1);
        return output;
}
