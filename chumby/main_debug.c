#include <stdio.h>

/* Main debug program, passing different arguments will allow you to debug different parts of the program
s - sonar
v - pwm control
c - compass
g - gps
*/

int sonar_debug()
{
	int sonar[2] = {0,0};
	int count = 0;
	int tty_sonar = initialize_i2c();

	//Initialization
	if (initialize_sonar(tty_sonar) < 0)
	{
		return 0;
	}

	//Main code
	while( count < 10 )
	{
		take_range(tty_sonar);
		usleep(60000);
		printf("Output from E0: %d\n", get_left_range(tty_sonar));
		//printf("Output from E2: %d\n", get_right_range(tty_sonar));
		
		count++;
	}

	close(tty_sonar);
	return 0;
}

int pwm_debug()
{
	initialize_motor();
	return 0;
}

int compass_debug()
{	
	int tty_compass = initialize_i2c();
	printf("Heading: %d",get_heading(tty_compass));
	close(tty_compass);
	return 0;
}

int gps_debug()
{
   	int tty_gps = initialize_gps();
   	char buf[100]; //not sure of size yet
   	read(tty_gps, buf, sizeof(buf));
   	return 0;
}


int main(int argc, char **argv)
{
	printf("ROBOMAGELLAN TEST PROGRAM\n");
	
	printf("Initialize\n");
	int i2c = initialize_i2c();
	initialize_motor();
	initialize_sonar(i2c);
	int count = 0;
	int dummy = 0;
	
	printf("Test Sonar\n");
	while (count < 10)
	{
		take_range(i2c);
		usleep(50000);
		printf("%d %d\n",get_left_range(i2c), get_right_range(i2c));
		count++;
	}
	
	count = 0;
	printf("Test Compass\n");
	while(count < 10)
	{
		printf("%d\n",get_heading(i2c));
		count++;
	}

	printf("Test GPS\n");
	printf("Test Camera\n");
	printf("Test Motor Control\n");
}
