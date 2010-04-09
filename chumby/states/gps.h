int initialize_gps();
void convert_latitude(char * lat, struct Location* position);
void convert_longitude(char * lon, struct Location* position);
void get_gps(int tty_gps, struct Location* position);

