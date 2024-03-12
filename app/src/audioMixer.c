#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "hal/wavePlayer.h" 
#include "timer.h"

#define BASS "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define HIHAT "wave-files/100053__menegass__gui-drum-cc.wav"


void beat_1(int bpm){
    if(bpm == 0){
        printf("test/n");
    }
    //pthread_t bassThread;
    //pthread_t snareThread;
    //pthread_t hihatThread;


}
