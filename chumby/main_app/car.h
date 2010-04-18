enum {
    CAR_NO_ERROR = 0,
    CAR_PWM_INIT_FAIL = -1,
    CAR_PWM_DRIVE_ERR = -2,
    CAR_PWM_TURN_ERR = -3,
    CAR_BAD_PARAM = -4,
    NUM_CAR_ERRORS = 5
} CAR_ERROR_CODES;

#define CAR_ERROR_STR(__err)  car_err_msgs[-1*__err]
extern const char * car_err_msgs[NUM_CAR_ERRORS];

int init_car();
int car_set_speed(int percent);
int car_set_turn(int percent);
