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
#define NUM_BEATS 3
#define MAX_SOUND_BITE 30
#define SHRT_MAX 32767
#define SHRT_MIN -32768
#define SAMPLE_RATE 44100


typedef struct
{
    wavedata_t *soundSamples;
    int position;
	int location;
    bool isFull;
} playData_t;

float volume = 0.8;
int bpm = 120;

static int currentBeat = 0;
int beatNumber = 1;
bool playerActive = true;
bool queueFull = false;
unsigned long playbackBufferSize = 0;
short *playbackBuffer = NULL;
static pthread_t playThread;
static pthread_t beatThread;
static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static wavedata_t sampleFiles[NUM_BEATS];
static playData_t soundBites[MAX_SOUND_BITE];
snd_pcm_t *handle;

//locks for thread synchronization
static void lock(){
    pthread_mutex_lock(&soundMutex);
}
static void unlock(){
    pthread_mutex_unlock(&soundMutex);
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
    handle = Audio_openDevice();
    printf("device open\n");
    Audio_readWaveFileIntoMemory(HIHAT,&sampleFiles[0]);
    Audio_readWaveFileIntoMemory(SNARE,&sampleFiles[1]);
    Audio_readWaveFileIntoMemory(BASS,&sampleFiles[2]);
    printf("BASS numsample = %d\n",sampleFiles[2].numSamples);
    pthread_create(&beatThread,NULL,&audioMixer_selectBeat,NULL);

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

void audioMixer_setbpm(int newBpm){
    bpm = newBpm;
    return;
}

void audioMixer_queueSound(wavedata_t* sample,int location,int position){

    //queue audio soundbite;
    bool soundQueued = false;
    lock();
    for(int i=0 ;i < MAX_SOUND_BITE;i++){
        if(soundBites[i].isFull == false){
            soundBites[i].soundSamples = sample;
            soundBites[i].location = location;
            soundBites[i].position = position;
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
        printf("start beat#%d\n",halfBeat);
    int halfBeatinMS = 60000 / bpm / 2;
        switch (beatNumber)
        {
        case 1:
            audioMixer_queueSound(&sampleFiles[0],0,halfBeat);
            if(halfBeat == 0){
                audioMixer_queueSound(&sampleFiles[2],0,halfBeat);
            }
            if(halfBeat == 2){
                audioMixer_queueSound(&sampleFiles[1],0,halfBeat);
            }
            break;
        default:
            break;
        }
        printf("beat number %d queued\n",halfBeat);
        sleepForMs(halfBeatinMS);
        halfBeat++;
        if(halfBeat>=4){
            halfBeat=0;
        }
        
    }
    return NULL;
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

void createBuffer(short *Buffer, int BufferSize){
    memset(Buffer,0,BufferSize);
    short newData;
    int beatOffset = 0;
    
    int halfBeatinMS = 60000 / bpm / 2;
    int halfBeatinSamples = (halfBeatinMS*SAMPLE_RATE) / 1000;

    for(int i = 0; i<MAX_SOUND_BITE; i++){
        lock();
        printf("mutex locked\n");
        if(soundBites[i].isFull){
            int offset = soundBites[i].location;
            int numSamples = (soundBites[i].soundSamples->numSamples)-offset;
            //bool fullAudioPlayed = true;
            if(currentBeat!=soundBites[i].position){
                beatOffset = beatOffset+(halfBeatinSamples-offset);

                currentBeat = soundBites[i].position;
            }

            for(int j = beatOffset; j < beatOffset+numSamples;j++){
                if(j<BufferSize){

                    newData = soundBites[i].soundSamples->pData[offset];
                    short temp = Buffer[j] + newData;

                    if(temp > 0 && Buffer[j] < 0 && newData < 0){
                        Buffer[j] = SHRT_MIN;
                    }else if (temp < 0 && Buffer[j] > 0 && newData > 0){
                        Buffer[j] = SHRT_MAX;
                    }else{
                        Buffer[j] = temp;    
                    }
                    //printf("loaded sample %d",newData);
                    offset++;
                }else{
                    unlock();
                    audioMixer_queueSound(soundBites[i].soundSamples,offset,currentBeat);
                    lock();
                    //fullAudioPlayed = false;
                    break;
                }
                
            }
            soundBites[i].soundSamples->pData = NULL;
            soundBites[i].location = 0;
            soundBites[i].position = 0;
            soundBites[i].isFull = false;
            
            
        }
        unlock();
        printf("unlocked\n");
    }
    printf("BUFFER COMPLETE\n");
    queueFull = false;
}

void* playBeat(){
    
        while(playerActive){
            printf("creating buffer\n");
            createBuffer(playbackBuffer,playbackBufferSize);
            wavedata_t bufferSample = {.pData = playbackBuffer,.numSamples = playbackBufferSize};
            Audio_setVolume(&bufferSample,volume);
            printf("playing buffer with %d samples\n",bufferSample.numSamples);
            Audio_playFile(handle, &bufferSample);

        }
    return NULL;
}
