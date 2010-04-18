#define PWM_MAX_RIGHT_TURN      0x0B9A
//#define PWM_MAX_FORWARD_SPEED   0x1270
#define PWM_MAX_FORWARD_SPEED   0x1240
#define PWM_IDLE_MIN            0x1288
//#define PWM_IDLE_CENTER         0x12A4
#define PWM_IDLE_STOP           0x12A4
#define PWM_IDLE_CENTER         0x11C3
#define PWM_IDLE_MAX            0x12C0
#define PWM_MAX_REVERSE_SPEED   0x12D8
#define PWM_MAX_LEFT_TURN       0x18BA

void pwm_init();
void pwm_turn(int value);
void pwm_drive(int value);
