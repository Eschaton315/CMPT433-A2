#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_
#include <stdbool.h>

void configPinGPIO();

void joystick_init();

bool joystickPressed(char *path);

void setJoystickValue();

void joystickListener_cleanup();

int joystick_getJoystickValue();

#endif