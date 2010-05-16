int SRF08_init(int addrs[], int num_addrs, int max_gain, int range);
int SRF08_do_ranging(int addrs[], int num_addrs, int mode);
int SRF08_read(int addrs[], int num_addrs, int mode, int * buf);
