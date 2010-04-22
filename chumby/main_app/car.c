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

static int speed;
static int turn;

int init_car() {
    if(pwm_init() < 0) // configure pwm pins
        return CAR_PWM_INIT_FAIL;
    if(pwm_drive(PWM_IDLE_STOP) < 0) // make sure wheels are not spinning
        return CAR_PWM_DRIVE_ERR;
    if(pwm_turn(PWM_CENTER) < 0) // center the wheels
        return CAR_PWM_TURN_ERR;
        
    return CAR_NO_ERROR;
}

int car_set_speed(int percent) {
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
        retval = (speed < 0) ?  (pwm_drive(PWM_IDLE_MIN)) : (pwm_drive(PWM_IDLE_MAX));
    }

    if(retval < 0)
        return CAR_PWM_DRIVE_ERR;

    speed = percent;

    return CAR_NO_ERROR;
}

int car_set_turn(int percent) { 
    int retval = 0;

    if(percent > 100 || percent < -100)
        return CAR_BAD_PARAM;

    // Turn right
    if(percent > 100) {
        retval = pwm_turn(PWM_CENTER - ((PWM_CENTER_MIN - PWM_MAX_RIGHT_TURN) * percent / 100));
    // Turn left
    } else if(percent < 100) {
        retval = pwm_turn(PWM_CENTER + -1 * ((PWM_MAX_LEFT_TURN - PWM_CENTER_MAX) * percent / 100));
    // Turn center
    } else if (!percent) {
        retval = pwm_turn(PWM_CENTER);
    }

    if(retval < 0)
        return CAR_PWM_TURN_ERR;

    turn = percent;

    return CAR_NO_ERROR;
}

int car_get_speed() {
    return speed;
}

int car_get_turn() {
    return turn;
}
