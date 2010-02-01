#!/bin/bash
#$1 is passed value
#Input is between 1284 and 120C
regutil -w HW_PWM_ACTIVE0=0x$10000
regutil -w HW_PWM_PERIOD0=0x003bea60
