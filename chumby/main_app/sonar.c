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
#include "sonar.h"

#define LEFT_SONAR_ADDR 0xE0
#define FRONT_SONAR_ADDR 0xE2
#define READ_BIT 0x1
#define RANGE 140
#define GAIN 0

// Must line up w/ SONAR_ERROR_CODES in
const char * sonar_err_msgs[NUM_SONAR_ERRORS] = {
    "NO_ERROR",
    "I2C_INIT_FAIL",
    "I2C_WRITE_ERR",
    "I2C_READ_ERR",
    "FRONT_SONAR_INIT_FAIL",
    "LEFT_SONAR_INIT_FAIL"
};

static int tty;

int sonar_init()
{
    int address = LEFT_SONAR_ADDR; //Address of 1st sensor
    int output=0;
    int retries=0;
    tty = initialize_i2c(); // This should be removed if other non-sonar devices on I2C Bus

    if (!tty)
        return I2C_INIT_FAIL;

//Make sure everything works, in this case read the software version #
    for (address=LEFT_SONAR_ADDR; address <= FRONT_SONAR_ADDR; address += 2)
    {
        output = 0;
        retries = 0;
        do
        {
            if(retries++ > 20) {
                close(tty);
                return (address == LEFT_SONAR_ADDR) ? LEFT_SONAR_INIT_FAIL : FRONT_SONAR_INIT_FAIL;
            }
            if(write_i2c(tty, address | READ_BIT, 0, 1, 0) < 0) {
                close(tty);
                return I2C_WRITE_ERR;
            }
            usleep(10000);
            if(read(tty,&output,1) < 0) {
                close(tty);
                return I2C_READ_ERR;
            }
        } 
        while (output != 10);
    }

    // Set new range(6m)
    address = LEFT_SONAR_ADDR;
    while ( address <= FRONT_SONAR_ADDR )
    {
        if(write_i2c(tty, address, 2, 1,RANGE) < 0) {
            close(tty);
            return I2C_WRITE_ERR;
        }
        if(read(tty,&output,1) < 0) {
            close(tty);
            return I2C_READ_ERR;
        }
        address= address + 2;
    }

    // Set new gain corresponding to range
    address = LEFT_SONAR_ADDR;
    while ( address <= FRONT_SONAR_ADDR )
    {
        if(write_i2c(tty, address, 1, 1,GAIN) < 0) {
            close(tty);
            return I2C_WRITE_ERR;
        }
        if(read(tty,&output,1) < 0) {
            close(tty);
            return I2C_READ_ERR;
        }
        address = address + 2;
    }
    
    return SONAR_NO_ERROR;
}

int sonar_take_range()
{
    int address = LEFT_SONAR_ADDR; //Address of 1st sensor
    int output = 0;
    // Take range
    while ( address <= FRONT_SONAR_ADDR )
    {
        if(write_i2c(tty, address, 0, 1,80) < 0) { 
            close(tty);
            return I2C_WRITE_ERR;
        }
        if(read(tty, &output, 1) < 0) {
            close(tty);
            return I2C_READ_ERR;
        }
        address = address + 2;
    }
    usleep(70000); //Delay for ranging to complete 
    return SONAR_NO_ERROR;
}

int sonar_get_left()
{
    int output = 0;
    if(write_i2c(tty, LEFT_SONAR_ADDR | READ_BIT, 3, 1, 0) < 0) {
        close(tty);
        return I2C_WRITE_ERR;
    }
    if(read(tty,&output,1) < 0) {
        close(tty);
        return I2C_READ_ERR;
    }
    return output;
}

int sonar_get_front()
{
    int output = 0;
    if(write_i2c(tty, FRONT_SONAR_ADDR | READ_BIT, 3, 1, 0) < 0) {
        close(tty);
        return I2C_WRITE_ERR;
    }
    if(read(tty, &output,1) < 0) {
        close(tty);
        return I2C_READ_ERR;
    }
    return output;
}
