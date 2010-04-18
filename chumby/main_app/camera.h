enum {
    CAMERA_NO_ERROR = 0,
    CAMERA_OPEN_FAIL = -1,
    CAMERA_WRITE_ERR = -2,
    CAMERA_READ_ERR = -3,
    CAMERA_START_TRACK_TIMEOUT = -4,
    CAMERA_STOP_TRACK_TIMEOUT = -5,
    CAMERA_CONE_POS_TIMEOUT = -6,
    NUM_CAMERA_ERRORS = 7
} CAMERA_ERROR_CODES;

#define CAMERA_ERROR_STR(__err)  camera_err_msgs[-1*__err]
// This is defined in camera.c
extern const char * camera_err_msgs[NUM_CAMERA_ERRORS];

int camera_init();
int camera_start_tracking();
int camera_stop_tracking();
int camera_cone_position();
