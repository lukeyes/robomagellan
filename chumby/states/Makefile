@echo "compiling init state..."
init_state: init_state.o usb_i2c.o sonar.o gps.o camera.o
	arm-linux-gcc init_state.o usb_i2c.o sonar.o gps.o camera.o

init_state.o: usb_i2c.h sonar.h sonar.h gps.h camera.h init_state.c
	arm-linux-gcc -c init_state.c

usb_i2c.o: usb_i2c.h usb_i2c.c
	arm-linux-gcc -c usb_i2c.c

sonar.o: sonar.h sonar.c
	arm-linux-gcc -c sonar.c

gps.o: gps.h gps.c
	arm-linux-gcc -c gps.c

camera.o: camera.h camera.c
	arm-linux-gcc -c camera.c


#all: oademo
#
#oademo: main.o usb_i2c.o sonar.o pwm_car.o drive_control.o
#	arm-linux-gcc main.o usb_i2c.o sonar.o pwm_car.o drive_control.o -o oademo

#main.o: sonar.h usb_i2c.h main.c 
#	arm-linux-gcc -c main.c 

#usb_i2c.o: usb_i2c.h usb_i2c.c
#	arm-linux-gcc -c usb_i2c.c

#sonar.o: sonar.h sonar.c
#	arm-linux-gcc -c sonar.c

#pwm_car.o: pwm_car.h pwm_car.c
#	arm-linux-gcc -c pwm_car.c

#drive_control.o: drive_control.h drive_control.c
#	arm-linux-gcc -c drive_control.c

#clean:
#	rm -rf *o oademo
