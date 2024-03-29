/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for car pwm signal control. Includes a tuning driver.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include "pwm.h"

static int *mem = 0;
static int fd = 0;
static int *prev_mem_range = 0;

/* functions for reading and writing protected memory */
int read_kernel_memory(long offset);
int write_kernel_memory(long offset, long value);

/******************************************************************************
 * read_kernel_memory
 *
 * params:
 *      long    offset - the offset of memory location to read
 *
 * desc:
 *      this function will return the 4-bytes at the specified
 *      memory offset of kernel memory
 *****************************************************************************/
int read_kernel_memory(long offset) {
    int result;

    int *mem_range = (int *)(offset & ~0xFFFF);
    if( mem_range != prev_mem_range ) 
    {
        prev_mem_range = mem_range;

        if(mem)
            munmap(mem, 0xFFFF);
        if(fd)
            close(fd);

        fd = open("/dev/mem", O_RDWR);
        if( fd < 0 ) {
            perror("Unable to open /dev/mem");
            fd = 0;
            return -1;
        }

        mem = mmap(0, 0xffff, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset&~0xFFFF);
        if( -1 == (int)mem ) {
            perror("Unable to mmap file");

            if( -1 == close(fd) )
                perror("Also couldn't close file");

            fd=0;
            return -1;
        }
    }

    int scaled_offset = (offset-(offset&~0xFFFF));
    result = mem[scaled_offset/sizeof(long)];

    return result;
}

/******************************************************************************
 * write_kernel_memory
 *
 * params:
 *      long    offset - the offset of memory location to write
 *
 * desc:
 *      this function will write the 4-byte value at the specified
 *      memory offset of kernel memory
 *****************************************************************************/
int write_kernel_memory(long offset, long value) {
    int old_value;
    int scaled_offset;
    old_value = read_kernel_memory(offset);
    if(old_value < 0)
        return -1;
    scaled_offset = (offset-(offset&~0xFFFF));
    mem[scaled_offset/sizeof(long)] = value;
    return 0;
}

/******************************************************************************
 * pwm_init
 *
 * params:
 *      na
 *
 * desc:
 *      configures the pwm pins
 *****************************************************************************/
int pwm_init()
{
    //Change mux to use pins and set to pwm output for PWM0 and PWM1
    if(write_kernel_memory(0x80018138,0x30f00000) < 0) //HW_PINCTRL_MUXSEL3_CLR=0x30f00000
        return -1;
    if(write_kernel_memory(0x80064004,0x00000013) < 0) //HW_PWM_CTRL_SET=0x00000013
        return -1;

    return 0;
}

/******************************************************************************
 * pwm_turn
 *
 * params:
 *      int value - pwm register setting for the "turning" pwm signal 
 *
 * desc:
 *      sets the "turning" pwm register to the specified value
 *****************************************************************************/
int pwm_turn(int value)
{
    if(write_kernel_memory(0x80064030,value << 16) < 0) //HW_PWM_ACTIVE1=0x(value)0000
        return -1;
    if(write_kernel_memory(0x80064040,0x003bea60) < 0) //HW_PWM_PERIOD1=0x003bea60
        return -1;

    return 0;
}

/******************************************************************************
 * pwm_drive
 *
 * params:
 *      int value - pwm register setting for the "drive" pwm signal 
 *
 * desc:
 *      sets the "drive" pwm register to the specified value
 *****************************************************************************/
int pwm_drive(int value)
{
    if(write_kernel_memory(0x80064010,value << 16) < 0) //HW_PWM_ACTIVE0=0x(value)0000
        return -1;
    if(write_kernel_memory(0x80064020,0x003bea60) < 0) //HW_PWM_PERIOD0=0x003bea60
        return -1;

    return 0;
}
#if 0
void exit_routine (int sig) {
    pwm_drive(PWM_IDLE_MAX);
    exit(0);
}

int main (void) {
    unsigned int speed;
    unsigned int turn;

    // Enable Control-C detection
    signal(SIGINT, exit_routine);

    pwm_init();
    fprintf(stderr,"speed(hex): ");
    fscanf(stdin,"%x", &speed);
    fprintf(stderr,"turn(hex): ");
    fscanf(stdin,"%x", &turn);
    pwm_drive(speed);
    pwm_turn(turn);
    while(1) {
        sleep(1);
    }
    return 0;
}
#endif
