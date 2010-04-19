struct Location 
{
    double latitude;
    double longitude;
};

int gps_init();
int gps_get_position(struct Location* position);
double calc_target_heading(struct Location* pos, struct Location* dest);
