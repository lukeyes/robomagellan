int sonar_init();
int sonar_take_range();
int sonar_get_left();
int sonar_get_front();

enum {
    NO_ERROR = 0,
    I2C_INIT_FAIL = -1,
    I2C_WRITE_ERR = -2,
    I2C_READ_ERR = -3,
    FRONT_SONAR_INIT_FAIL = -4,
    LEFT_SONAR_INIT_FAIL = -5,
} SONAR_ERROR_CODES;
