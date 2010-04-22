#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"
#include "sonar.h"
#include "camera.h"
#include "gps.h"
#include "compass.h"

void done_state() {
    car_set_speed(0);
    car_set_speed(0);
    car_set_speed(0);
    car_set_turn(0);
    if (debug)
        system("stty echo");

    sonar_close();
    camera_close();
    gps_close();
    compass_close();

    return;
}
//641
//6EF
