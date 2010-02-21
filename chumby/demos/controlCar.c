/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    A simple program that demos drive control of the 
 *          Traxxas E-MAXX RC truck.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* Driving Directions */
#define FORWARD 0x0
#define STOP    0x1
#define REVERSE 0x2

/* Driving Speeds */
#define SPEED0  0x0
#define SPEED1  0x1
#define SPEED2  0x2
#define SPEED3  0x3
#define SPEED4  0x4

/* orientation */
#define LEFT    0x0
#define CENTER  0x1
#define RIGHT   0x2

typedef unsigned char   u8;
typedef unsigned short  u16;

unsigned int speed = 0x1270;
unsigned int rturn = 0x0B9A;
unsigned int lturn = 0x18BA;
unsigned int center = 0x12c0;

/* Maps speeds to register value setting */
u16 speedToRegvals [5][2] = {
//   forward    reverse
    {0x1234,    0x1234},
    {0x1234,    0x1234},
    {0x1234,    0x1234},
    {0x1234,    0x1234},
    {0x1234,    0x1234},
};

/* Car control functions */
void drive(u8 direction);
void turn(u8 orientation);
void increaseSpeed();
void decreaseSpeed();

struct {
    u8 speed;
    u8 direction;
    u8 orientation;
} carState;

static int *mem = 0;
static int fd = 0;
static int *prev_mem_range = 0;

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

int write_kernel_memory(long offset, long value) {
    int old_value = read_kernel_memory(offset);
    int scaled_offset = (offset-(offset&~0xFFFF));
    mem[scaled_offset/sizeof(long)] = value;
    return 0;
}

int initialize_motor()
{
    //Change mux to use pins and set to pwm output for PWM0 and PWM1
    write_kernel_memory(0x80018138,0x30f00000); //HW_PINCTRL_MUXSEL3_CLR=0x30f00000
    write_kernel_memory(0x80064004,0x00000013); //HW_PWM_CTRL_SET=0x00000013

    //Put forward drive into neutral
    write_kernel_memory(0x80064010,0x12C00000); //HW_PWM_ACTIVE0=0x12C00000
    write_kernel_memory(0x80064020,0x003bea60); //HW_PWM_PERIOD0=0x003bea60
    return 0;
}

int pwm_turn(int value)
{
    write_kernel_memory(0x80064030,value << 16); //HW_PWM_ACTIVE1=0x(value)0000
    write_kernel_memory(0x80064040,0x003bea60); //HW_PWM_PERIOD1=0x003bea60
    return 0;
}

int pwm_speed(int value)
{
    write_kernel_memory(0x80064010,value << 16); //HW_PWM_ACTIVE0=0x(value)0000
    write_kernel_memory(0x80064020,0x003bea60); //HW_PWM_PERIOD0=0x003bea60
    return 0;
}

void drive(u8 direction) {
    switch(direction) {
        case FORWARD:
            if(carState.direction == REVERSE) {
                pwm_speed(center);
                carState.direction = STOP;
            } else if(carState.direction == STOP) {
                pwm_speed(speed);
                carState.direction = FORWARD;
            }
            break;
        case REVERSE:
	    if(carState.direction == FORWARD) {
	    	pwm_speed(center);
	    	carState.direction = STOP;
	    }
            break;
        case STOP:
            //TODO: stop the car
            pwm_speed(center);
            carState.direction = STOP;
            break;
    }
}
void turn(u8 orientation) {
    switch(orientation) {
        case LEFT:
            if(carState.orientation == RIGHT) {
                //TODO: orient to center
                pwm_turn(center);
                carState.orientation = CENTER;
            } else if(carState.orientation == CENTER) {
                //TODO: orient to LEFT
                pwm_turn(lturn);
                carState.orientation = LEFT;
            }
            break;
        case CENTER:
            //TODO: orient to center
            pwm_turn(center);
            carState.orientation = CENTER;
            break;
        case RIGHT:
            if(carState.orientation == LEFT) {
                //TODO: orient to center
                pwm_turn(center);
                carState.orientation = CENTER;
            } else if(carState.orientation == CENTER) {
                //TODO: orient to right 
                pwm_turn(rturn);
                carState.orientation = RIGHT;
            }
            break;
    }
}
void increaseSpeed() {
}
void decreaseSpeed() {
}

int main (void) {
    char key = 0;
    char lastKey = 0;

    carState.speed = 0;
    carState.direction = STOP;
    carState.orientation = CENTER;

	pwm_speed(center);
	pwm_turn(center);
    initialize_motor();
    while (1) {
        key = (char)getc(stdin);
        switch(key) {
            case 'w': // Drive forward
                printf("you pressed %c\n", key);
                drive(FORWARD);
                break;
            case 's': // Drive in reverse
                printf("you pressed %c\n", key);
                drive(REVERSE);
                break;
            case 'a': // Turn left
                printf("you pressed %c\n", key);
                turn(LEFT);
                break;
            case 'd': // Turn right
                printf("you pressed %c\n", key);
                turn(RIGHT);
                break;
            case '-': // Decrease speed
                printf("you pressed %c\n", key);
                decreaseSpeed();
                break;
            case '=': // Increase speed
                printf("you pressed %c\n", key);
                increaseSpeed();
                break;
            case 'x': // Stop
                printf("you pressed %c\n", key);
                drive(STOP);
                break;
        }
    }
    return 0;
}
