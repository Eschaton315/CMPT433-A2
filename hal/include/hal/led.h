#ifndef _LED_H_
#define _LED_H_

#include <stdbool.h>

bool ledOff(char *ledPosition);

bool ledOn(char *ledPosition);

void resetLED();

void OnLED();

#endif