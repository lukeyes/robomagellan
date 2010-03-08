/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for driving the Traxxas E-MAXX RC truck.
 */

#include <stdio.h>
#include <string.h>
#include "pwm_car.h"
#include "driveControl.h"

void driveForward(int percent) {
    pwm_drive((PWM_MAX_FORWARD_SPEED - PWM_IDLE) * percent / 100);
}

void driveReverse(int percent) {
    pwm_drive((PWM_IDLE - PWM_MAX_REVERSE_SPEED) * percent / 100);
}

void stop() {
    pwm_drive(PWM_IDLE);
}

void turnRight(int percent) {
    pwm_turn((PWM_MAX_RIGHT_TURN - PWM_IDLE) * percent / 100)
}

void turnLeft(int percent) { 
    pwm_turn((PWM_IDLE - PWM_MAX_LEFT_TURN) * percent / 100)
}

void turnCenter() {
    pwm_turn(PWM_IDLE);
}

void initCar() {
    pwm_init(); // configure pwm pins
	pwm_speed(PWM_IDLE); // make sure wheels are not spinning
	pwm_turn(PWM_IDLE); // center the wheels
}
