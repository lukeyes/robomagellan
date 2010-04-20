enum {
    GPS_NO_ERROR = 0,
    GPS_OPEN_FAIL = -1,
    GPS_DATA_TIMEOUT = -2,
    GPS_READ_TIMEOUT = -3,
    GPS_DATA_INVALID = -4,
    NUM_GPS_ERRORS = 5
} GPS_ERROR_CODES;

#define GPS_ERROR_STR(__err)  gps_err_msgs[-1*__err]
// This is defined in gps.c
extern const char * gps_err_msgs[NUM_GPS_ERRORS];

struct Location 
{
    double latitude;
    double longitude;
};

int gps_init();
int gps_get_position(struct Location* position);
double calc_target_heading(struct Location* pos, struct Location* dest);
double get_relative_heading(double compass, double target);
