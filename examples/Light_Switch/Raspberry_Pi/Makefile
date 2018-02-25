CC = gcc
CFLAGS = -Wall -Werror -c
WIRINGPI = -lwiringPi

main:
	gcc ../../../src/Linux/unitooth_L2CAP.c ../../../src/Linux/unitooth_RFCOMM.c light_switch.c  -lwiringPi -lbluetooth -lpthread -o light_switch

