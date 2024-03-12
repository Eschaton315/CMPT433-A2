#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_
#include <stdbool.h>

void configPinGPIO();

bool joystickPressed(char *path);

int isJoystickPressed();

#endif