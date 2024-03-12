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
    //bool active = false;
    int bpm = BASE_BPM;
    float volume = 0.8; 

    if(bpm!=0){
        printf("bpm = %d\n",bpm);
    }

    //config joytick
    //configPinGPIO();
    joystick_init();
    configPinI2C();
   
    //FOR JOYSTICK UP and DOWN for volume control LEFT RIGHT temp used to play audio
    while(getJoystickValue()!=5){
        //printf("button is %d\n",getJoystickValue());
        if(getJoystickValue() == 1){
            if(volume<1){
                volume = volume+0.05;
                printf("Volume set to %0.0f\n",volume*100);
            }else{
                printf("Already at Max Volume\n");
            }
            sleepForMs(300);
        }
        if(getJoystickValue() == 2){
            if(volume>0){
                volume = volume-0.05;
                printf("Volume set to %0.0f\n",(volume*(100))+0.01);
            }else{
                printf("Already at Min Volume\n");
            }
            sleepForMs(300);
            
        }
        if(getJoystickValue() == 3){
            //active = true;
            path = "wave-files/100059__menegass__gui-drum-snare-soft.wav";
            //play audio
            wavePlayer_play(path,volume);
        }
        if(getJoystickValue() == 4){
            //active = true;
            path = "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav";
            //play audio
            wavePlayer_play(path,volume);
        }

        if(getJoystickValue() == 5){
            
            break;
        }
        
        //sleepForMs(1000);
        
    
    }

    joystickListener_cleanup();

    printf("EXITING\n");

    return 0;
}