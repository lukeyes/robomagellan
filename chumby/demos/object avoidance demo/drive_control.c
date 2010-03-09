/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for driving the Traxxas E-MAXX RC truck.
 */

#include <stdio.h>
#include <string.h>
#include "pwm_car.h"
#include "drive_control.h"

typedef unsigned char   u8;
typedef unsigned short  u16;

void drive_forward(int percent) {
    pwm_drive((PWM_MAX_FORWARD_SPEED - PWM_IDLE) * percent / 100);
}

void drive_reverse(int percent) {
    pwm_drive((PWM_IDLE - PWM_MAX_REVERSE_SPEED) * percent / 100);
}

void stop() {
    pwm_drive(PWM_IDLE);
}

void turn_right(int percent) {
    pwm_turn((PWM_MAX_RIGHT_TURN - PWM_IDLE) * percent / 100);
}

void turn_left(int percent) { 
    pwm_turn((PWM_IDLE - PWM_MAX_LEFT_TURN) * percent / 100);
}

void turn_center() {
    pwm_turn(PWM_IDLE);
}

void init_car() {
    pwm_init(); // configure pwm pins
	pwm_drive(PWM_IDLE); // make sure wheels are not spinning
	pwm_turn(PWM_IDLE); // center the wheels
}
