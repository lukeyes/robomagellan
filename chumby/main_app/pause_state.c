#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"

void pause_state() {
    car_set_speed(0);
    car_set_turn(0);
    while (kill_switch_asserted) {
        sleep(1);
    }
    next_state = prev_state;
}
