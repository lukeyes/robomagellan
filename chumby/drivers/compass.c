#define COMPASS_ADDRESS 0x42

int get_heading(int tty)
{
	unsigned char headPtr[2] = {0,0};
	int heading = 0;

	//Read heading
	write_i2c(tty,COMPASS_ADDRESS + 1,0x41,2,0);
	read(tty,&headPtr, 2);
	heading = (headPtr[0] << 8) + headPtr[1];
	heading /= 10;
	
	return heading;
}
