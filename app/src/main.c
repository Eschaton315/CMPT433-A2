#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "audioMixer.h"
#include "hal/joyStick.h"
#include "hal/wavePlayer.h"
#include "udp.h"
#include "timer.h" 
#include "terminate.h"

#define BASE_BPM 120
#define BASE_VOLUME 0.8
#define MAX_BEAT 3

int main(){

    //config joytick
    //configPinGPIO();
    joystick_init();
    configPinI2C();
    audioMixer_init();
    UDPThreadCreate();
    
   
   long long time = getTimeInMs();
   long long newTime = 0;

   while(!getTerminateStatus()){
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
    newTime = getTimeInMs();
    if(newTime>=time+1000){
        float minTime = getMinTime();
        float maxTime = getMaxTime();
        float avg = (minTime+maxTime)/2;
        printf("M%d %dbpm Audio[%0.3f,%0.3f] avg %0.3f/%lu Accel[,] avg /\n",audioMixer_getBeat(),audioMixer_getBpm(),minTime,maxTime,avg,getBufferNum());
        time = newTime;
    }
   }
   
    joystickListener_cleanup();
    audioMixer_cleanup();

    printf("EXITING\n");

    return 0;
}