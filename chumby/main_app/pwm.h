#define PWM_MAX_RIGHT_TURN      0x0B9A
#define PWM_MAX_FORWARD_SPEED   0x1224
#define PWM_CENTER              0x119B
#define PWM_IDLE_MIN            0x1288
#define PWM_IDLE_MAX            0x12C0
#define PWM_MAX_REVERSE_SPEED   0x1324
#define PWM_MAX_LEFT_TURN       0x18BA

int pwm_init();
int pwm_turn(int value);
int pwm_drive(int value);
