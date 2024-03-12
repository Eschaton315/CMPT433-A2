#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "hal/joyStick.h"
#include "hal/wavePlayer.h"
#include "timer.h" 

#define BASE_BPM 120


int main(){
    char* path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
    bool active = false;
    int bpm = BASE_BPM;

    if(bpm!=0){
        printf("bpm = %d\n",bpm);
    }

    //config joytick
    //configPinGPIO();
    joystick_init();
    configPinI2C();
   

    while(getJoystickValue()!=5){
        //printf("button is %d\n",getJoystickValue());
        if(getJoystickValue() == 1){
            active = true;
            path = "wave-files/100051__menegass__gui-drum-bd-hard.wav";
        }
        if(getJoystickValue() == 2){
            active = true;
            path = "wave-files/100053__menegass__gui-drum-cc.wav";
        }
        if(getJoystickValue() == 3){
            active = true;
            path = "wave-files/100059__menegass__gui-drum-snare-soft.wav";
        }
        if(getJoystickValue() == 4){
            active = true;
            path = "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav";
        }

        if(getJoystickValue() == 5){
            break;
        }
        
        if(active&&getJoystickValue()!=0){
            //play audio
            wavePlayer_play(path);
        }
        //sleepForMs(1000);
        
    
    }

    joystickListener_cleanup();

    printf("EXITING\n");

    return 0;
}