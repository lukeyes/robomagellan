//init all peripherals
//verify all peripherals
//output status to screen
//receive gps coordinates
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "device_threads.h"
#include "sonar.h"
#include "compass.h"
#include "camera.h"
#include "gps.h"

void init_state() {
    static int first = 1;
    int retval;

    if(first) {
        if(debug)
            spawn_debug_thread();

#ifdef USE_SONAR
        // initialize sonar sensors
        retval = sonar_init();
        if(retval != SONAR_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
#endif

#ifdef USE_COMPASS
        //initialize compass
        retval = compass_init();
        if(retval != COMPASS_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
#endif

#ifdef USE_CAMERA
        //initialize camera
        retval = camera_init();
        if(retval != CAMERA_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), CAMERA_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
        fprintf(stderr,"camera_init returned: %s\n", CAMERA_ERROR_STR(retval));
        retval = camera_start_tracking();
        if(retval != CAMERA_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), CAMERA_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
        fprintf(stderr,"camera_start_tracking returned: %s\n", CAMERA_ERROR_STR(retval));
#endif
        // Spawn device threads
        spawn_device_threads();
        first = 0;
    }
    next_state = NAVIGATION_STATE;
}
