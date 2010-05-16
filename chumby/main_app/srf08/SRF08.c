/*  Author: Michael Ortiz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  SRF08.c
 *  A general purpose driver for SRF08 Ultrasonic Rangefinder.
 *  For I2C address changing see ../utils/SRF08_addr_util.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "usb_i2c.h"

//SRF08 registers
#define SW_REV_REG          0   //RO
#define COMMAND_REG         0   //WO
#define LIGHT_SENSOR_REG    1   //RO
#define MAX_GAIN_REG        1   //WO
#define RANGE_REG           2   //WO
//valid echo numbers: 1-17
#define ECHO_HI_BYTE(_echoNum) ((int)(_echoNum)*2) //R0
#define ECHO_LO_BYTE(_echoNum) ((int)(_echoNum)*2 + 1) //RO

//SRFO8 Commands
#define RANGING_MODE_IN 0x50//inches
#define RANGING_MODE_CM 0x51//centimeters
#define RANGING_MODE_US 0x52//mircoseconds
//_mode (0-2)
#define RANGING_MODE(_mode) (0x50 + _mode)

//SRF08 Constants
#define RANGE_STEP_SIZE 0.043
#define RANGE_TIME_RATIO (65/11) // milliseconds/meters

static int range_time; //microseconds

/* SRF08_init
 *
 * addrs []     addresses of the SRF08s to apply settings to
 * num_addrs    number of addresses (ie number of SRF08s)
 * max_gain     the maximum analogue gain for echos (0-31)
 * range        detection range in meters (6m max)
 */
int SRF08_init(int addrs[], int num_addrs, int max_gain, int range) {
    int i;
    int resp;
    // Apply the settings to all specified SRF08s
    for(i=0; i<num_addrs; ++i) {
        // set the gain
        usb_i2c_write(addrs[i], MAX_GAIN_REG, 1, max_gain);
        usb_i2c_read(&resp, 1);
        
        // set the range
        usb_i2c_write(addrs[i], RANGE_REG, 1, (int)((double)range/0.043));
        usb_i2c_read(&resp, 1);
    }

    // Calculate range time
    range_time = (RANGE_TIME_RATIO * range + 5)*1000;

    return 0;
}

/* SRF08_do_ranging
 *
 * addrs []     addresses of the SRF08s to apply settings to
 * num_addrs    number of addresses (ie number of SRF08s)
 * mode         inches(0), centimeters(1), microseconds(2) 
 */
int SRF08_do_ranging(int addrs[], int num_addrs, int mode) {
    int i;
    int resp;
    // Have all specified SRF08s initiate a ranging
    for(i=0; i<num_addrs; ++i) {
        // initiate a ranging
        usb_i2c_write(addrs[i], COMMAND_REG, 1, RANGING_MODE(mode));
        usb_i2c_read(&resp, 1);
    }
    usleep(range_time*2);
    return 0;
}

/* SRF08_read
 *
 * addrs []     addresses of the SRF08s to apply settings to
 * num_addrs    number of addresses (ie number of SRF08s)
 * mode         inches(0), centimeters(1), microseconds(2) 
 * buf          ranging results of each SRF08
 */
int SRF08_read(int addrs[], int num_addrs, int mode, int * buf) {
    int i;

    // Read ranging results from all specified SRF08s
    for(i=0; i<num_addrs; ++i) {
        // tell SRF08 to send ranging results
        usb_i2c_write(addrs[i] | I2C_READ_BIT, ECHO_LO_BYTE(1), 1, 0);
        // read ranging results
        usb_i2c_read(&buf[i], 1);
    }
    return 0;
}

#if 0
int main () {
    int addr=0xE0;
    int dist=0;
    usb_i2c_init();
    SRF08_init(&addr, 1, 0, 6);
    while(1) {
        SRF08_do_ranging(&addr, 1, 0);
        SRF08_read(&addr, 1, 0, &dist);
        fprintf(stderr, "dist = %d\n", dist);
    }
    return 0;
}
#endif
