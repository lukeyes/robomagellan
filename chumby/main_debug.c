#include <stdio.h>

int main()
{
	int tty = initialize_i2c();
	int sonar[2] = {0,0};
	int count = 0;

	//Initialization
	if (initialize_sonar(tty) < 0)
	{
		return 0;
	}

	//Main code
	while( count < 30 )
	{
		take_range(tty);
		usleep(60000);
		get_range(tty, sonar);
		printf("Ouput: %d %d\n", sonar[0], sonar[1] );
		count++;
	}

	close(tty);
	return 0;
}
