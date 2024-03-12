#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hal/joyStick.h"
#include "hal/wavePlayer.h" 

int main(){
    char* path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
    bool active = false;

    //config joytick
    configPinGPIO();
    configPinI2C();

    wavePlayer_play(path);


    while(isJoystickPressed()!=2){
        if(isJoystickPressed() == 1){
            active = true;
            path = "wave-files/100056__menegass__gui-drum-cyn-hard.wav";
        }
        if(isJoystickPressed() == 3){
            active = true;
            path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
        }
        if(isJoystickPressed() == 4){
            active = true;
            path = "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav";
        }

        if(isJoystickPressed() == 2){
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