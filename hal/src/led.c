#include "hal/led.h"
#include <stdio.h>
#include <string.h>


#define TRIGGER1 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define TRIGGER2 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define TRIGGER3 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define TRIGGER4 "/sys/class/leds/beaglebone:green:usr3/trigger"

bool ledOff(char* Trigger){

    FILE *pLedTriggerFile = fopen(Trigger, "w");
    if (pLedTriggerFile == NULL) {
        printf("ERROR OPENING %s.", Trigger);
        return false;
        }
    int charWritten = fprintf(pLedTriggerFile, "none");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        return false;
        }
    fclose(pLedTriggerFile);     
    return true;
}

bool ledOn(char* Trigger){

    FILE *pLedTriggerFile = fopen(Trigger, "w");
    if (pLedTriggerFile == NULL) {
        printf("ERROR OPENING %s.", Trigger);
        return false;
        }
    int charWritten = fprintf(pLedTriggerFile, "default-on");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        return false;
        }
    fclose(pLedTriggerFile);     
    return true;
}

void resetLED(){
    ledOff(TRIGGER1);
    ledOff(TRIGGER2);
    ledOff(TRIGGER3);
    ledOff(TRIGGER4);
}

void OnLED(){
    ledOn(TRIGGER1);
    ledOn(TRIGGER2);
    ledOn(TRIGGER3);
    ledOn(TRIGGER4);
    
}