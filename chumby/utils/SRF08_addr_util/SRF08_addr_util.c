/*      A module that will set the attached SRF08 to the
 *      specified address.
 *
 *  Author:     Michael Ortiz (mtortiz.mail@gmail.com)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "usb_i2c.h"

#define BROADCAST_ADDR 0X00

//SRF08 registers
#define SW_REV_REG  0   //RO
#define COMM_REG    0   //WO

//Address change command sequence
#define FIRST_ADDR_CHANGE_COMM 0XA0
#define SECOND_ADDR_CHANGE_COMM 0XAA
#define THIRD_ADDR_CHANGE_COMM 0XA5

//I2C related
#define I2C_READ_BIT 0X01

static int tty=0;

int main()
{
    unsigned int cur_addr=0;
    unsigned int new_addr=0;
    unsigned int sw_rev=0;

    tty = initialize_i2c(); // This should be removed if other non-sonar devices on I2C Bus

    if (tty < 0) {
        fprintf(stderr, "I2C failed to initialized\n");
        return -1;
    }

    fprintf(stderr, "Enter new address (0XE0 - 0XFE): ");
    fscanf(stdin, "%x", &new_addr);
    
    if(new_addr & 0x01) {
        fprintf(stderr, "Address must be even\n");
        close(tty);
        return -1;
    }

    fprintf(stderr, "Identifying current address...\n");
    for(cur_addr = 0XE0; cur_addr <= 0xFE; cur_addr += 2) {
        //ping SRF08 for software revision #
        if(write_i2c(tty, cur_addr | I2C_READ_BIT, SW_REV_REG, 1, 0) < 0) {
            fprintf(stderr, "I2C write fail\n");
            close(tty);
            return -1;
        }

        if(read(tty,&sw_rev,1) < 0) {
            fprintf(stderr, "I2C read fail\n");
            close(tty);
            return -1;
        } else if (sw_rev != 0xFF) {
            break;
        }
    }

    if(sw_rev == 0x255) {
        fprintf(stderr, "Could not identify current address\n");
        close(tty);
        return -1;
    }

    fprintf(stderr, "Current address: 0X%X\n", cur_addr);
    fprintf(stderr, "Software Revision: %d\n", sw_rev);
    fprintf(stderr, "Changing address from 0X%X to 0X%X...\n", cur_addr, new_addr);

    //Set the new address
    if(write_i2c(tty, cur_addr, COMM_REG, 1, FIRST_ADDR_CHANGE_COMM) < 0) {
        fprintf(stderr, "I2C write fail\n");
        close(tty);
        return -1;
    }
    if(write_i2c(tty, cur_addr, COMM_REG, 1, SECOND_ADDR_CHANGE_COMM) < 0) {
        fprintf(stderr, "I2C write fail\n");
        close(tty);
        return -1;
    }
    if(write_i2c(tty, cur_addr, COMM_REG, 1, THIRD_ADDR_CHANGE_COMM) < 0) {
        fprintf(stderr, "I2C write fail\n");
        close(tty);
        return -1;
    }
    if(write_i2c(tty, cur_addr, COMM_REG, 1, new_addr) < 0) {
        fprintf(stderr, "I2C write fail\n");
        close(tty);
        return -1;
    }

    fprintf(stderr, "Address changed successfully\n");
    
    close(tty);
    return 0;
}
