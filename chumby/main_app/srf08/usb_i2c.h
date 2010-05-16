#define I2C_BROADCAST_ADDR 0X00
#define I2C_READ_BIT 0X01

int usb_i2c_init();
int usb_i2c_write(int address, int reg, int nbytes, int data);
int usb_i2c_read(int * buf, int size);
