#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "car.h"

void done_state() {
    car_set_turn(0);
    car_set_speed(0);
    next_state = DONE_STATE;
    return;
}
