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
int temp = 0;
int ltemp = 0;

void drive_forward(int percent) {
    pwm_drive(PWM_IDLE_MIN - ((PWM_IDLE_MIN - PWM_MAX_FORWARD_SPEED) * percent / 100));
    temp = (PWM_IDLE_MIN - ((PWM_IDLE_MIN - PWM_MAX_FORWARD_SPEED) * percent / 100));
}

void drive_reverse(int percent) {
    pwm_drive(PWM_IDLE_MAX+((PWM_IDLE_MAX - PWM_MAX_REVERSE_SPEED) * percent / 100));
}

void stop_car() {
    pwm_drive(PWM_IDLE_STOP);
}

void turn_left(int percent) {
    pwm_turn(PWM_IDLE_MAX+((PWM_MAX_LEFT_TURN-PWM_IDLE_MAX) * percent / 100));
    ltemp = PWM_IDLE_MAX+((PWM_MAX_LEFT_TURN-PWM_IDLE_MAX) * percent / 100);
}

void turn_right(int percent) { 
    pwm_turn(PWM_IDLE_MIN-((PWM_IDLE_MIN - PWM_MAX_RIGHT_TURN) * percent / 100));
}

void turn_center() {
    pwm_turn(PWM_IDLE_CENTER);
}

void init_car() {
    pwm_init(); // configure pwm pins
    pwm_drive(PWM_IDLE_STOP);
    pwm_turn(PWM_IDLE_CENTER);
}
