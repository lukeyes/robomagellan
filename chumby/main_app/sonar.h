enum {
    SONAR_NO_ERROR = 0,
    I2C_INIT_FAIL = -1,
    I2C_WRITE_ERR = -2,
    I2C_READ_ERR = -3,
    FRONT_SONAR_INIT_FAIL = -4,
    RIGHT_SONAR_INIT_FAIL = -5,
    NUM_SONAR_ERRORS = 6,
} SONAR_ERROR_CODES;

#define SONAR_ERROR_STR(__err)  sonar_err_msgs[-1*__err]
// This is defined in sonar.c
extern const char * sonar_err_msgs[NUM_SONAR_ERRORS];

int sonar_init();
int sonar_take_range();
int sonar_get_right();
int sonar_get_front();
