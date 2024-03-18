#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "timer.h"
#include "hal/wavePlayer.h"
#include "hal/joyStick.h" 
#include "timer.h"

#define BASS "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define HIHAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define CYN "wave-files/100057__menegass__gui-drum-cyn-soft.wav"
#define TOMHI "wave-files/100062__menegass__gui-drum-tom-hi-hard.wav"
#define TOMLO "wave-files/100064__menegass__gui-drum-tom-lo-hard.wav"
#define NUM_BEATS 6
#define MAX_SOUND_BITE 50
#define SHRT_MAX 32767
#define SHRT_MIN -32768
#define SAMPLE_RATE 44100


typedef struct
{
    wavedata_t *soundSamples;
	int location;
    bool isFull;
} playData_t;

float volume = 0.8;
int bpm = 120;
int beatNumber = 1;

//static int currentBeat = 0;
long long baseTime;
long long minTime;
long long maxTime;
bool playerActive = true;
bool queueFull = false;
unsigned long playbackBufferSize = 0;
short *playbackBuffer = NULL;
static pthread_t playThread;
static pthread_t beatThread;
static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static wavedata_t sampleFiles[NUM_BEATS];
static playData_t soundBites[MAX_SOUND_BITE];
//static playData_t overflowBites[MAX_SOUND_BITE];
snd_pcm_t *handle;

//locks for thread synchronization
static void lock(){
    pthread_mutex_lock(&soundMutex);
}
static void unlock(){
    pthread_mutex_unlock(&soundMutex);
}

float getMinTime(){
    float minSec = (float)minTime;
    return minTime/1000;
}

float getMaxTime(){
    float maxSec = (float)maxTime;
    return maxTime/1000;
}


//prototype
void* audioMixer_selectBeat();
void* playBeat();

//TODO: make volume and bpm adjusters

//initialize the mixer and call needed threads
void audioMixer_init(){
    printf("bpm = %d\n",bpm);
    for(int i; i<MAX_SOUND_BITE;i++){
        soundBites[i].isFull = false;
    }
    baseTime = getTimeInMs();
    handle = Audio_openDevice();
    //printf("device open\n");
    Audio_readWaveFileIntoMemory(HIHAT,&sampleFiles[0]);
    Audio_readWaveFileIntoMemory(SNARE,&sampleFiles[1]);
    Audio_readWaveFileIntoMemory(BASS,&sampleFiles[2]);
    Audio_readWaveFileIntoMemory(CYN,&sampleFiles[3]);
    Audio_readWaveFileIntoMemory(TOMHI,&sampleFiles[4]);
    Audio_readWaveFileIntoMemory(TOMLO,&sampleFiles[5]);
    //printf("BASS numsample = %d\n",sampleFiles[2].numSamples);
    pthread_create(&beatThread,NULL,&audioMixer_selectBeat,NULL);
    //sleepForMs(100);
    unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
    //playbackBufferSize = 44100;
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));
    //printf("buffersize = %lu\n", playbackBufferSize);
    pthread_create(&playThread,NULL,&playBeat,NULL);

    return;
}

void audioMixer_setVol(float newVol){
    volume = newVol;
    return;
}

float audioMixer_getVol(){
    return volume;
}

void audioMixer_setBpm(int newBpm){
    bpm = newBpm;
    return;
}

int audioMixer_getBpm(){
    return bpm;
}

void audioMixer_setBeat(int newBeatNum){
    beatNumber = newBeatNum;
    return;
}

int audioMixer_getBeat(){
    return beatNumber;
}

void audioMixer_queueSound(wavedata_t* sample,int location){

    //queue audio soundbite;
    bool soundQueued = false;
    lock();
    for(int i=0 ;i < MAX_SOUND_BITE;i++){
        if(soundBites[i].isFull == false){
            soundBites[i].soundSamples = sample;
            soundBites[i].location = location;
            soundBites[i].isFull = true;
            soundQueued = true;
            if(i == MAX_SOUND_BITE-1){
                queueFull = true;
            }
            break;
        }
    }
    unlock();
    if(!soundQueued){
        printf("ERROR SOUND NOT QUEUED\n");
    }else{
       printf("queue sound success\n");
    }
}

void* audioMixer_selectBeat(){
    int halfBeat = 0;
    while(playerActive){
        //printf("start beat# %d\n",halfBeat);
    //int halfBeatinMS = 60000 / bpm / 2;
        switch (beatNumber)
        {
        case 1:
            audioMixer_queueSound(&sampleFiles[0],0);
            if(halfBeat == 0){
                audioMixer_queueSound(&sampleFiles[2],0);
            }
            if(halfBeat == 2){
                audioMixer_queueSound(&sampleFiles[1],0);
            }
            break;
        case 2:
            audioMixer_queueSound(&sampleFiles[3],0);
            audioMixer_queueSound(&sampleFiles[4],0);
            if(halfBeat==3){
                audioMixer_queueSound(&sampleFiles[5],0);
            }
            break;
            break;
        default:
            break;
        }
        //printf("beat number %d queued\n",halfBeat);
        //sleepForMs(halfBeatinMS);
        halfBeat++;
        if(halfBeat>=4){
            halfBeat=0;
        }
    sleepForMs(60000 / bpm / 2);  
    }
    return NULL;
}

void audioMixer_selectSound(char* path){
    if(path == BASS){
        audioMixer_queueSound(&sampleFiles[2],0);  
    }
    if(path == SNARE){
        audioMixer_queueSound(&sampleFiles[2],0);  
    }
    if(path == HIHAT){
        audioMixer_queueSound(&sampleFiles[2],0);  
    }

}

void audioMixer_cleanup(){
    playerActive = false;
    pthread_join(beatThread, NULL);
    pthread_join(playThread,NULL);
    free(playbackBuffer);
    playbackBuffer = NULL;
    wavePlayer_cleanup(handle);

    return;
}

void createBuffer(){
    
    memset(playbackBuffer,0,playbackBufferSize);
    short newData;
    //int beatOffset = 0;
    //int overFlowCount =0;
    
    //int halfBeatinMS = 60000 / bpm / 2;
   // int halfBeatinSamples = (halfBeatinMS*SAMPLE_RATE) / 1000;



    for(int i=0 ; i<(int)playbackBufferSize;i++){
        lock();
        int pcmVal = 0;
        //printf("locked\n");
        for(int j=0; j<MAX_SOUND_BITE;j++){
           if(soundBites[j].isFull==true){
            int location = soundBites[j].location;
            //printf("location = %d\n",location);
            newData = soundBites[j].soundSamples->pData[location]; 
            short temp = pcmVal + newData;
                if(temp > 0 && pcmVal < 0 && newData < 0){
                    pcmVal = SHRT_MIN;
                }else if (temp < 0 && pcmVal > 0 && newData > 0){
                    pcmVal = SHRT_MAX;
                }else{
                    pcmVal= temp;    
                }
            soundBites[j].location++;
            if(soundBites[j].location>=soundBites[j].soundSamples->numSamples){
                soundBites[j].isFull = false;
                soundBites[j].soundSamples = NULL;
            }
            }
        }
        //printf("%d ",pcmVal);
        playbackBuffer[i] = pcmVal;
        unlock();
        //printf("unlocked\n");
    }

    queueFull = false;
}

void* playBeat(){
    
        while(playerActive){
           // printf("creating buffer\n");
            minTime = getTimeInMs()-baseTime; 
            createBuffer();
            maxTime = getTimeInMs()-baseTime; 
            wavedata_t bufferSample = {.pData = playbackBuffer,.numSamples = playbackBufferSize};
            Audio_setVolume(&bufferSample,volume);
            //printf("playing buffer with %d samples\n",bufferSample.numSamples);
            Audio_playFile(handle, &bufferSample);

        }
    return NULL;
}
