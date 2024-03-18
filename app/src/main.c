#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "audioMixer.h"
#include "hal/joyStick.h"
#include "hal/wavePlayer.h"
#include "timer.h" 

#define BASE_BPM 120
#define BASE_VOLUME 0.8
#define MAX_BEAT 3

int main(){
    //char* path = "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav";
    //bool active = false;
    int bpm = BASE_BPM;
   // float volume = BASE_VOLUME; 

    if(bpm!=0){
        printf("bpm = %d\n",bpm);
    }

    //config joytick
    //configPinGPIO();
    joystick_init();
    configPinI2C();
    audioMixer_init();
   
   long long time = getTimeInMs();

   while(getTimeInMs()<time+7000){
    int joyStickVal = joystick_getJoystickValue();
    float volume = audioMixer_getVol();
    int bpm = audioMixer_getBpm();
    int beatNum = audioMixer_getBeat();
    switch (joyStickVal)
    {
    case 1:
        
        if(volume<1){
            volume = volume+0.05;
            printf("Volume set to %0.0f\n",volume*100);
        }else{
            printf("Already at Max Volume\n");
        }
        audioMixer_setVol(volume);  
        sleepForMs(300);
        
        break;
    case 2:
        
        if(volume>0){
            volume = volume-0.05;
            printf("Volume set to %0.0f\n",(volume*(100))+0.01);
        }else{
            printf("Already at Min Volume\n");
        }
        audioMixer_setVol(volume);            
        sleepForMs(300);

        break;
    case 3:
        
        if(bpm>40){
            bpm  = bpm-5;
            printf("tempo set to %d\n",bpm);
        }else{
            printf("Already at Min tempo\n");
        }
        audioMixer_setBpm(bpm);            
        sleepForMs(300);

        break;
    case 4:
        
        if(bpm<300){
            bpm  = bpm+5;
            printf("tempo set to %d\n",bpm);
        }else{
            printf("Already at MAX tempo\n");
        }
        audioMixer_setBpm(bpm);               
        sleepForMs(300);
        break;
    case 5:
        
        beatNum = beatNum+1;
        if(beatNum==MAX_BEAT){
            beatNum = 0;
        }
        audioMixer_setBeat(beatNum);
        break;

    default:
        break;
    }
   }
    //FOR JOYSTICK UP and DOWN for volume control LEFT RIGHT temp used to play audio
    /*
    while(joystick_getJoystickValue()!=5){
        //printf("button is %d\n",joystick_getJoystickValue());
        if(joystick_getJoystickValue() == 1){
            if(volume<1){
                volume = volume+0.05;
                printf("Volume set to %0.0f\n",volume*100);
            }else{
                printf("Already at Max Volume\n");
            }
            sleepForMs(300);
        }
        if(joystick_getJoystickValue() == 2){
            if(volume>0){
                volume = volume-0.05;
                printf("Volume set to %0.0f\n",(volume*(100))+0.01);
            }else{
                printf("Already at Min Volume\n");
            }
            sleepForMs(300);
            
        }
        if(joystick_getJoystickValue() == 3){
            //active = true;
            path = "wave-files/100059__menegass__gui-drum-snare-soft.wav";
            //play audio
            wavePlayer_play(path,NULL,volume);
        }
        if(joystick_getJoystickValue() == 4){
            //active = true;
            path = "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav";
            //play audio
            wavePlayer_play(path,NULL,volume);
        }

        if(joystick_getJoystickValue() == 5){
            
            break;
        }
        
        //sleepForMs(1000);
        
    
    }
    */
    //sleepForMs(10000);
    joystickListener_cleanup();

    printf("EXITING\n");

    return 0;
}