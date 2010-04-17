/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for driving the Traxxas E-MAXX RC truck.
 */

#include <stdio.h>
#include <string.h>
#include "pwm.h"
#include "car.h"

// Must line up w/ CAR_ERROR_CODES in
const char * car_err_msgs[NUM_CAR_ERRORS] = {
    "NO_ERROR",
    "CAR_PWM_INIT_FAIL",
    "CAR_PWM_DRIVE_ERR",
    "CAR_PWM_TURN_ERR",
    "CAR_BAD_PARAM"
};

int init_car() {
    if(pwm_init() < 0) // configure pwm pins
        return CAR_PWM_INIT_FAIL;
    if(pwm_drive(PWM_IDLE_CENTER) < 0) // make sure wheels are not spinning
        return CAR_PWM_DRIVE_ERR;
    if(pwm_turn(PWM_IDLE_CENTER) < 0) // center the wheels
        return CAR_PWM_TURN_ERR;
        
    return CAR_NO_ERROR;
}

int car_drive(int percent) {
    int retval = 0;

    if(percent > 100 || percent < -100)
        return CAR_BAD_PARAM;

    // Drive forward
    if(percent > 0) {
        retval = pwm_drive(PWM_IDLE_MIN - ((PWM_IDLE_MIN - PWM_MAX_FORWARD_SPEED) * percent / 100));
    // Drive reverse
    } else if (percent < 0) {
        retval = pwm_drive(PWM_IDLE_MAX + -1 * ((PWM_IDLE_MAX - PWM_MAX_REVERSE_SPEED) * percent / 100));
    // Stop car
    } else if (!percent) {
        retval = pwm_drive(PWM_IDLE_CENTER);
    }

    if(retval < 0)
        return CAR_PWM_DRIVE_ERR;

    return CAR_NO_ERROR;
}

int car_turn(int percent) { 
    int retval = 0;

    if(percent > 100 || percent < -100)
        return CAR_BAD_PARAM;

    // Turn right
    if(percent > 100) {
        retval = pwm_turn(PWM_IDLE_MIN-((PWM_IDLE_MIN - PWM_MAX_RIGHT_TURN) * percent / 100));
    // Turn left
    } else if(percent < 100) {
        retval = pwm_turn(PWM_IDLE_MAX+((PWM_MAX_LEFT_TURN-PWM_IDLE_MAX) * percent / 100));
    // Turn center
    } else if (!percent) {
        retval = pwm_turn(PWM_IDLE_CENTER);
    }

    if(retval < 0)
        return CAR_PWM_TURN_ERR;

    return CAR_NO_ERROR;
}
