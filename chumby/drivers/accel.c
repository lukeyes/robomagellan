#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <string.h>

#include "mma7455.h"

//////////////////////////////////////////////
//// I2C I/O functions
//////////////////////////////////////////////

#define I2C_FILE_NAME "/dev/i2c-0"


static int I2C_Read_nbyte(int i2c_file, unsigned char address,
                          unsigned char reg, unsigned char *inbuf, int length) {

    unsigned char outbuf[1];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    outbuf[0] = reg;

    messages[0].addr    = address;
    messages[0].flags   = 0;
    messages[0].len     = sizeof(outbuf);
    messages[0].buf     = outbuf;

    messages[1].addr    = address;
    messages[1].flags   = I2C_M_RD;
    messages[1].len     = length;
    messages[1].buf     = inbuf;

    packets.msgs = messages;
    packets.nmsgs = 2;

    if(ioctl(i2c_file, I2C_RDWR, &packets) < 0) {
        perror("Unable to write/read data");
        return 1;
    }

    return 0;
}
   
static int I2C_Write_nbyte(int i2c_file, unsigned char address,
                           unsigned char reg, unsigned char *outbuf,
                           int length) {

    char buf[length+1];

    // Begin by setting the accelerometer's current register to X.
    if (ioctl(i2c_file, I2C_SLAVE, address) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        perror("Unable to assign slave address");
        return 1;
    }

    // Write to register 0x07, the MODE register.
    buf[0] = reg;
    memcpy(&(buf[1]), outbuf, length);
    if(write(i2c_file, buf, length+1) != length+1) {
        perror("Unable to write value");
        return 1;
    }
    return 0;
}

static int I2C_Write_1byte(int file, unsigned char address, unsigned char reg,
                           unsigned char data) {
    return I2C_Write_nbyte(file, address, reg, &data, 1);
}



static int set_accelerator_register(int file, unsigned char reg, unsigned char value) {
    return I2C_Write_1byte(file, ACCEL_I2C_ADDR, reg, value);
}



static int initialize_accelerometer() 
{
    int file = open(I2C_FILE_NAME, O_RDWR);
    unsigned char regs[0x0A];
    int reg;

    if(I2C_Read_nbyte(file, ACCEL_I2C_ADDR, 0, regs, sizeof(regs))) {
        perror("Unable to dump accelerometer registers");
        return 1;
    }

    // set standby, 2g range
    if(set_accelerator_register(file, MMA7455_MODE_REG, 0x4)) {
        fprintf(stderr, "Unable to disable the accelerometer\n");
        return 1;
    }


    // disable detections, set digital filter bandwidth to 62.5 Hz
    // (125 Hz data output rate)
    set_accelerator_register(file, MMA7455_CTL1, 0x38);
    // set drive strength to standard
    set_accelerator_register(file, MMA7455_CTL2, 0x0);


    // Finally, re-enable the accelerometer by setting measurement mode.
    if(set_accelerator_register(file, MMA7455_MODE_REG, 0x4 | 0x1)) {
        fprintf(stderr, "Unable to enable the accelerometer\n");
        return 1;
    }


    return file;
}



static int real_read_accelerometer(int file, int xyz[3]) {
    signed char buf[3];

    // Read the three register values.
    if(I2C_Read_nbyte(file, ACCEL_I2C_ADDR, MMA7455_XOUT8,
                      (unsigned char *)buf, sizeof(buf))) {
        perror("Unable to read acceleromter data");
        return 1;
    }

    // converts from 8-bit 2's compliment to unsigned int
    xyz[0] = buf[0] + 128;
    xyz[1] = buf[1] + 128;
    xyz[2] = buf[2] + 128;

    return 0;
}


// Reads the accelerometer and returns the result in *data.
// Notice how it ignores the return value from real_read_accelerometer.
// This is because failures to read are normal, because the timeout is so
// short.  We average the values, so missed errors shouldn't be a big deal.
double read_accelerometer(int file) {
    int total_readings = 4;
    int successes = 0;
    int errors = 0;
    int read_number = 0;
    unsigned int xyz[3], xyz_sum[3];
    double accel = 0;

    // Obtain /total_readings/ samples from the accelerometer.  Some may
    // fail, which is alright.  We'll average everything out to obtain a
    // smoother sample.
    // Average out the good reads.
    xyz_sum[0]   = 0;
    xyz_sum[1]   = 0;
    xyz_sum[2]   = 0;
    for(read_number=0; read_number<total_readings; read_number++) {
        if(real_read_accelerometer(file, xyz))
            errors++;
        else {
            successes++;
            xyz_sum[0] += xyz[0];
            xyz_sum[1] += xyz[1];
            xyz_sum[2] += xyz[2];
        }
	usleep(10000);
    }
    accel = xyz_sum[0] / successes;
    accel = accel - 128;
    accel = .5 * accel;
    return accel;
}

/*
int main()
{
	struct accelReadData ai;
	int count = 0;
	i2c_file = initialize_accelerometer();
	while(count <= 10)
	{
		printf("%d\n", read_acceleromter(i2c_file));
		usleep(50000);
		count++;
	}
	close(i2c_file);
	return 0;
}*/

