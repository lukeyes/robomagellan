#include <stdio.h>
#include "drivers/drive_control.h"
#include "drivers/camera.h"

#define LEFT_THRESHOLD = 65
#define RIGHT_THRESHOLD = 95
#define HIT_THRESHOLD = 2
/*
int cone_find(int camera_tty)
{
	int found = 0;
	start_tracking(camera_tty);
	printf("Start tracking\n");
	//Turn in a circle, assume the cone is found
	while(found == 0)
	{
		//drive_forward(10);
		//turn_right(100);
		fprintf(stderr,"Going in a circle.\n");
		found = cone_position(camera_tty);
		fprintf(stderr,"Middle Pixel: %d\n",found);
	}
	//drive_center();
	fprintf(stderr,"Going Straight\n");
}

int cone_hit(int camera_tty, int sonar_tty)
{	
	int touch = 0;
	int range = 0;
	int direction = 0;
	while(touch < 1)
	{	
		direction = cone_position(camera_tty); 
		if(direction < LEFT_THRESHOLD)
		{
			//turn_left(10);
			fprintf(stderr,"Turn Left");
		}
		if(direction > RIGHT_THRESHOLD)
		{
			//turn_right(10);
			fprintf(stderr,"Turn Right");
		}
		take_range(sonar_tty);
		usleep(100000);
		//turn_center();
		fprintf(stderr, "Going Straight\n");
		range = get_front_range(sonar_tty);
		printf("Range: %d\n",range);
		//if(range < HIT_THRESHOLD)
		//	touch = 1;

	}
	printf("Hit cone!");
	//stop_car();
	return 0;
}

*/

int main()
{
	int camera_tty = initialize_camera();
	start_tracking(camera_tty);
	fprintf(stderr,"Camera initialized\n");
	while(1){
		fprintf(stderr,"Middle Pixel Location: %d",cone_position(camera_tty));
	}
	close(camera_tty);
	return 0;
}
