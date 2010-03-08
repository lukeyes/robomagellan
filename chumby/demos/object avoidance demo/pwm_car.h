#define PWM_MAX_FORWARD_SPEED   0x1270
#define PWM_MAX_REVERSE_SPEED   0xFFFF // TODO: get real definintion
#define PWM_IDLE                0x12C0
#define PWM_MAX_RIGHT_TURN      0x0B9A
#define PWM_MAX_LEFT_TURN       0x18BA

void pwm_init();
void pwm_turn(int value);
void pwm_drive(int value);
