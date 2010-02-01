#!/bin/sh

#Change mux to use pins and set to pwm output for PWM0 and PWM1
regutil -w HW_PINCTRL_MUXSEL3_CLR=0x30f00000
regutil -w HW_PWM_CTRL_SET=0x0000013

#Put forward drive into neutral
regutil -w HW_PWM_ACTIVE0=0x12C00000
regutil -w HW_PWM_PERIOD0=0x003bea60

