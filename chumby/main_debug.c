#include <stdio.h>

/* Main debug program, passing different arguments will allow you to debug different parts of the program
s - sonar
v - pwm control
c - compass
g - gps
*/

int sonar_debug()
{
	int tty_sonar = initialize_i2c();
	int sonar[2] = {0,0};
	int count = 0;

	//Initialization
	if (initialize_sonar(tty_sonar) < 0)
	{
		return 0;
	}

	//Main code
	while( count < 30 )
	{
		take_range(tty_sonar);
		usleep(60000);
		printf("Output from E0: %d\n", get_left_range(tty_sonar));
		printf("Output from E2: %d\n", get_right_range(tty_sonar));
		
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
	return 0;
}

int gps_debug()
{
   int TTY = initialize_gps();
   double data[BUFSIZ];
   char buf[BUFSIZ];
   char buftrim[70];
   char word[7][50];
   char * pStr;
   char * tmp;
   int i;
   int res;
   /*Variables needed from $GPRMC string*/
   char status[BUFSIZ];
   float longitude;
   //char long_direction;
   float latitude;
   //char lat_direction;
   float bearing;
   float current_direction;
   int j;
while(1){
    j=0;
    res = read(TTY, buf, BUFSIZ);
    for(j=0;j<sizeof(buf);j++)
    {
        if(buf[j] == '$' && buf[j+5] == 'C')
            break;
        else
          buf[j]=' ';
    }

//      fprintf(stderr, "%c %c %c %c %c",buf[j],buf[j+1],buf[j+2],buf[j+3],buf[j+4]);
      for(i=j;i<j+70;i++)
      {
//        fprintf(stderr,"%c",buf[i]);
         if(buf[i]==',')
            buf[i] = ' ';
         buftrim[i-j] = buf[i];
//         fprintf(stderr,"%c",buf[i]);
      }
//      fprintf(stderr,"%s\n\n",buftrim);
      sscanf(buftrim,"%*s %*f %s %f %*s %f %*s %f %f %*s %*s %*s %*s",status,&latitude,&longitude,&bearing,&current_direction);

      data[0] = latitude;
      data[1] = longitude;
      fprintf(stderr,"latitude: %f\n",data[0]);
      fprintf(stderr,"longitude: %f\n",data[1]);
//      fprintf(stderr,"%s --- %f --- %f --- %f --- %f\n",status,latitude,longitude,bearing,current_direction);

//      printf("%s", buf);
    sleep(1);  
 }
	return 0;
}


int main(int argc, char **argv)
{
	int sonar = 0;
	int pwm = 0;
	int compass = 0;
	int gps = 0;
	int ch = 0;
	while ((ch = getopt(argc, argv, "svcg")) != -1) 
	{
		switch(ch)
		{
			case 's':
				sonar = 1;
				break;
			case 'v':
				pwm = 1;
				break;
			case 'c':
				compass = 1;
				break;
			case 'g':
				gps = 1;
				break;
		}
	}
		
	if (sonar == 1)
		sonar_debug();
	if (pwm == 1)
		pwm_debug();
	if (compass == 1)
		compass_debug();
	if (gps == 1)
		gps_debug();

	return 0;
}
