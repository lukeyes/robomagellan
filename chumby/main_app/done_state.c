#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"
#include "sonar.h"
#include "camera.h"
#include "gps.h"
#include "compass.h"
#include "switch.h"

void done_state() {
    car_set_speed(0);
    car_set_speed(0);
    car_set_speed(0);
    car_set_turn(0);

    switch_close();
    sonar_close();
    camera_close();
    gps_close();
    compass_close();

    return;
}
//641
//6EF
