/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for car pwm signal control
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "drive_control.h"

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
    int old_value = read_kernel_memory(offset);
    int scaled_offset = (offset-(offset&~0xFFFF));
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
void pwm_init()
{
    //Change mux to use pins and set to pwm output for PWM0 and PWM1
    write_kernel_memory(0x80018138,0x30f00000); //HW_PINCTRL_MUXSEL3_CLR=0x30f00000
    write_kernel_memory(0x80064004,0x00000013); //HW_PWM_CTRL_SET=0x00000013
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
void pwm_turn(int value)
{
    write_kernel_memory(0x80064030,value << 16); //HW_PWM_ACTIVE1=0x(value)0000
    write_kernel_memory(0x80064040,0x003bea60); //HW_PWM_PERIOD1=0x003bea60
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
void pwm_drive(int value)
{
    write_kernel_memory(0x80064010,value << 16); //HW_PWM_ACTIVE0=0x(value)0000
    write_kernel_memory(0x80064020,0x003bea60); //HW_PWM_PERIOD0=0x003bea60
}
