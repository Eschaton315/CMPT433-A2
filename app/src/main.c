#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hal/joyStick.h"
#include "hal/wavePlayer.h" 

#define BASE_BPM 120

int main(){
    char* path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
    bool active = false;
    int bpm = BASE_BPM;

    if(bpm!=0){
        printf("bpm = %d\n",bpm);
    }

    //config joytick
    configPinGPIO();
    configPinI2C();

    while(isJoystickPressed()!=5){
        if(isJoystickPressed() == 1){
            active = true;
            path = "wave-files/100056__menegass__gui-drum-cyn-hard.wav";
        }
        if(isJoystickPressed() == 2){
            active = true;
            path = "wave-files/100058__menegass__gui-drum-snare-hard.wav";
        }
        if(isJoystickPressed() == 3){
            active = true;
            path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
        }
        if(isJoystickPressed() == 4){
            active = true;
            path = "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav";
        }

        if(isJoystickPressed() == 5){
            break;
        }
        
        if(active&&isJoystickPressed()!=0){
            //play audio
            wavePlayer_play(path);
        }

        
    
    }

    printf("EXITING\n");

    return 0;
}