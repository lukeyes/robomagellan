enum {
    COMPASS_NO_ERROR = 0,
    COMPASS_OPEN_TTY_FAIL = -1,
    COMPASS_TTY_READ_FAIL = -2,
    COMPASS_DATA_CORRUPT = -3,
    COMPASS_DATA_TIMEOUT = -4,
    NUM_COMPASS_ERRORS = 5
} COMPASS_ERROR_CODES;


#define COMPASS_ERROR_STR(__err)  compass_err_msgs[-1*__err]
// This is defined in sonar.c
extern const char * compass_err_msgs[NUM_COMPASS_ERRORS];

int compass_init();
double compass_get_heading();
