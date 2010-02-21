#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
//sleep(1);
   struct termios camera_term, old_term;
   char buf[BUFSIZ];
   int screen, tty;
   tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

   /*Set serial port settings*/
   tcgetattr(tty, &old_term);
   camera_term.c_cflag = B115200;
   camera_term.c_cflag |= CS8;
   camera_term.c_cflag |= CREAD;
   camera_term.c_iflag = IGNPAR | IGNBRK;
   camera_term.c_cflag |= CLOCAL;

   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&camera_term);
   /*Camera command strings*/
//   char config[] = "cr 18 32 19 33\r";
   char gs[] = "om 0 0\r";
   char servo[] = "sm 15\r";
//   char config[] = "cr 18 44 19 33\r";
   char track[] = "tc 144 194 37 87 0 41 \r";
//   char invert[] = "ti 1\r";
   char noise[] = "nf 5\r";
//   char output[] = "GT\r";
//   char enter[] = "\r";
   char get_servo[] = "gs 0\r";
//   char pm[] = "pm 2\r";
//   char win[] = "sp 16 4 5 30 15 5\r";
//   char move[] = "sv 1 45\r";
   /*Write commands*/
//   write(tty,&move,strlen(move)+1);
//   write(tty,&config,strlen(config)+1);
   write(tty,&gs,strlen(gs)+1);
//   sleep(1);
   write(tty,&servo,strlen(servo)+1);
//   write(tty,&noise,strlen(noise)+1);
//   write(tty,&pm,strlen(pm)+1);
//   write(tty,&win,strlen(win)+1);
   write(tty,&track,strlen(track)+1);

	int res, i;
	char angle[5];
	while(1)
	{
		char buf[BUFSIZ];
		res = read(tty,buf,BUFSIZ);
		for (i=0;i<strlen(buf);i++)
		{
			if(buf[0]=='T')
			{
				sscanf(buf,"%*s %s ",angle);
				fprintf(stderr,"%s\n",angle);
//			fprintf(stderr,"%c%c%c\n",buf[2],buf[3],buf);
//			if(buf[0]=='T');
//				fprintf(stderr,"%c%c%c"buf[1],buf[2],buf[3]);	
			}
		}
		usleep(50000);
	}
//   write(tty,&output,strlen(output)+1);
//   write(tty,&invert,strlen(invert)+1);

}
/*
   int flag = 0;
   while(1)
   {
      int res,i;
      char buf[BUFSIZ];
      res = read(tty,buf,BUFSIZ);
      for(i=0;i<strlen(buf);i++)
      {
*/
 /**/ //       fprintf(stderr,"%c",buf[i]);
/*         if(res>0)
         {
            if(buf[0]=='T')
            {
               flag= 1;
               for(i=0;i<res;i++)
               {
                  angle[i]=buf[i];
               }
            }
            else
               break;
            if(flag==1)
            {
               angle[i]='\0';
               break;
            }
         }
     }
     if(flag==1)
      break;
  }
   tcsetattr(tty,TCSANOW,&camera_term);
   close(tty);




//////////////////////

//return tty;
}
*/
/*void read_camera_angle(char *angle , int tty)
{

   int flag = 0;
   while(1)
   {
      printf("read angle loop\n");
      int res,i;
      char buf[BUFSIZ];

      res = read(tty,buf,BUFSIZ);
      for(i=0;i<strlen(buf);i++)
      {
         if(res>0)
         {
            printf("length is: %ld\n",strlen(buf));
            printf("value is: %c\n",buf[0]);
            if(buf[0]=='T')
            {
               flag= 1;
               for(i=0;i<res;i++)
               {
                  angle[i]=buf[i];
               }
            }
            else
               break;
            if(flag==1)
            {
               angle[i]='\0';
               break;
            }
         }
     }
     if(flag==1)
      break;
  }
}*/

/*int calculate_next_direction(int angle)
{
//   fprintf(stderr,"calculate function:%ld\n",angle);
   const int T_LEFT = 120;
   const int T_MIDLEFT = 117;
   const int STRAIGHT = 115;
   const int T_MIDRIGHT = 113;
   const int T_RIGHT = 110;

   struct range {
      int lower;
      int upper;
               };
   struct range left, midleft, straight, midright, right;

   left.upper = 97;
   left.lower = 46;

   midleft.upper = 122;
   midleft.lower = 98;

   straight.upper = 132;
   straight.lower = 123;

   midright.upper = 157;
   midright.lower = 133;

   right.upper = 210;
   right.lower = 158;

   int turn_num = STRAIGHT; //PWM int

   if(angle >= left.lower && angle <= left.upper)
      turn_num =  T_LEFT;

   if(angle >= midleft.lower && angle <= midleft.upper)
      turn_num = T_MIDLEFT;

   if(angle >= straight.lower && angle <= straight.upper)
      turn_num = STRAIGHT;

   if((angle >= midright.lower) && (angle <= midright.upper))
      turn_num = T_MIDRIGHT;

   if((angle >= right.lower) && (angle <= right.upper))
      turn_num = T_RIGHT;
//   fprintf(stderr,"before returning: %c",turn_num);
   return turn_num;
}*/
