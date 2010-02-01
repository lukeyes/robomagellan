#!/bin/bash
#$1 is passed value
#Input is between 0B9A(far right) and 18BA(far left)
regutil -w HW_PWM_ACTIVE1=0x$10000
regutil -w HW_PWM_PERIOD1=0x003bea60
