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
	int numSamples;
	short *pData;
} wavedata_t;

typedef struct
{
    wavedata_t *soundSamples;
    int position;
	int location;
} playData_t;

float volume = 0.8;
int bpm = 120;

static int soundQueuePosition = 0;
static int bufferPosition = 0;
static int bufferOffset = 0;
static int currentBeat = 0;
static int beatRestTime = 0;
int beatNumber = 0;
bool playerActive = true;
unsigned long playbackBufferSize = 0;
short *playbackBuffer = NULL;
static pthread_t playThread;
static pthread_t beatThread;
static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static playData_t soundBites[MAX_SOUND_BITE];


static void lock(){
    pthread_mutex_lock(&soundMutex);
}
static void unlock(){
    pthread_mutex_unlock(&soundMutex);
}


//static void *audioMixer_playBeat();
//void* beat_1();


void createBuffer(unsigned long BufferSize, short *Buffer){
    memset(Buffer,0,BufferSize);
    short newData;
    int count = 0;
    
    //allocate needed amount;
     //make sure its thread safe
    
    int halfBeatinMS = 60000 / bpm / 2;
    int halfBeatinSamples = (halfBeatinMS*SAMPLE_RATE) / 1000;

    for(int i = 0; i<MAX_SOUND_BITE; i++){
        if(soundBites[i].soundSamples){
            int offset = soundBites[i].location;
            int numSamples = soundBites[i].soundSamples->numSamples-offset;
            if(currentBeat!=soundBites[i].position){
                beatRestTime = beatRestTime + (60 * SAMPLE_RATE / bpm / 2) ;
                currentBeat = soundBites[i].position;
            }
            for(int j = beatRestTime; j < beatRestTime+numSamples;j++){
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
                    offset++;
                }else{
                    //create new insert to soundbites
                    int overflowOffset = (beatRestTime+numSamples)-BufferSize;
                    short *overflowData = malloc(overflowOffset);
                    for(int x = 0; x < overflowOffset;x++){
                        newData = soundBites[i].soundSamples->pData[offset];
                        short temp = Buffer[j] + newData;

                        if(temp > 0 && Buffer[j] < 0 && newData < 0){
                            overflowData[x] = SHRT_MIN;
                        }else if (temp < 0 && Buffer[j] > 0 && newData > 0){
                            overflowData[x] = SHRT_MAX;
                        }else{
                            overflowData[x] = temp;    
                        }
                        offset++; 
                    }
                    //figure out how the offset works n shit
                    break;
                }
                
            }

        }

        beatRestTime = 0; // or whatever the offset is
    }



    for(int i=bufferOffset;i<MAX_SOUND_BITE+bufferOffset;i++){
        if(i>=MAX_SOUND_BITE){
            i=i-MAX_SOUND_BITE;
        }
        if(soundBites[i].soundSamples){
            int offset = soundBites[i].location;
            int numSamples = soundBites[i].soundSamples->numSamples;
            //realloc(Buffer, numSamples+offset*size);
            if(offset+numSamples<BufferSize){
                for(int j = offset; j<numSamples;i++){
  
                newData = soundBites[i].soundSamples->pData[count];
                short temp = Buffer[j] + newData;

                if(temp > 0 && Buffer[j] < 0 && newData < 0){
                    Buffer[j] = SHRT_MIN;
                }else if (temp < 0 && Buffer[j] > 0 && newData > 0){
                    Buffer[j] = SHRT_MAX;
                }else{
                    Buffer[j] = temp;    
                }

                count++;
                }    
            }else{
                if(((offset+numSamples) - BufferSize)>bufferOffset)
                bufferOffset = (offset+numSamples) - BufferSize;
                 
            }
            count = 0;
        }
    }
    if(bufferOffset>0){
        for(int i=0;i<bufferOffset;i++){

        }
    }
}

void audioMixer_queueSound(wavedata_t sample,int location,int position){
    //make proper queue system
    soundBites[soundQueuePosition].soundSamples = &sample;
    soundBites[soundQueuePosition].location = location;
    soundBites[soundQueuePosition].position = position;
    soundQueuePosition++;
    if(soundQueuePosition>=MAX_SOUND_BITE){
        soundQueuePosition = 0;
    }
}

void audioMixer_selectBeat(wavedata_t samples[NUM_BEATS]){
    int halfBeat = 0;

    while(!playerActive){
    int halfBeatinMS = 60000 / bpm / 2;
    int halfBeatinSamples = (halfBeatinMS*SAMPLE_RATE) / 1000;
    lock();
        switch (beatNumber)
        {
        case 1:
            //audioMixer_queueSound(samples[0],halfBeat*halfBeatinSamples);
            audioMixer_queueSound(samples[0],0,halfBeat);
            if(halfBeat == 0){
                //audioMixer_queueSound(samples[2],halfBeat*halfBeatinSamples);
                audioMixer_queueSound(samples[2],0,halfBeat);
            }
            if(halfBeat == 2){
                //audioMixer_queueSound(samples[1],halfBeat*halfBeatinSamples);
                audioMixer_queueSound(samples[1],0,halfBeat);
            }
            break;
        default:
            break;
        }
        halfBeat++;
        if(halfBeat>=4){
            halfBeat=0;
        }
    }
    unlock();
}

void audioMixer_init(){
    printf("bpm = %d\n",bpm);
    wavedata_t sampleFiles[NUM_BEATS];

    //pthread_create(&mixerThread, NULL,&audioMixer_playBeat ,NULL);
    //handle = Audio_openDevice();
    snd_pcm_t *handle = Audio_openDevice();

    Audio_readWaveFileIntoMemory(HIHAT,&sampleFiles[0]);
    Audio_readWaveFileIntoMemory(SNARE,&sampleFiles[1]);
    Audio_readWaveFileIntoMemory(BASS,&sampleFiles[2]);
    audioMixer_selectBeat(sampleFiles);

    unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

    
    if(!playerActive){
        //play first beat
         pthread_create(&playThread,NULL,&playBeat,NULL);
        playerActive = true;
    }/*
    while(playerActive){

        if(joystick_getJoystickValue()==5){
            beatNumber++;
            if(beatNumber>=MaxNumber){
                beatNumber = 0;
            }
            printf("switching beat");
            switchBeat = true;
            pthread_cancel(beatThread);
            //switch statement for beats
            switch (beatNumber){

                case 1:
                    pthread_create(&beatThread,NULL,&beat_1,NULL);
                    break;

                case 2:
                    //beat 2
                    break;

                default:
                    //no beat
                    break;
            }
            switchBeat = false;
        }

    }
    */

    return;
}

/*
static void *audioMixer_playBeat(){
    
    
}
*/

void audioMixer_cleanup(){
    playerActive = false;
    //pthread_join(mixerThread, NULL);
    //wavePlayer_cleanup(handle);

    return;
}
/*
void* beat_1(){
    pthread_t playThread;
    playData_t playArgs;
    playArgs.playVolume = &volume;
    while(!switchBeat){
        playArgs.inst1 = HIHAT;
        playArgs.inst2 = BASS;
        int halfBeat = 60 / bpm / 2 * 1000;
        pthread_create(&playThread,NULL,&wavePlayer_play,(void *)&playArgs) ;
        //wavePlayer_play(HIHAT,BASS,volume);
        playArgs.inst2 = NULL; 
        sleepForMs(halfBeat);
        pthread_create(&playThread,NULL,&wavePlayer_play,(void *)&playArgs);
        //wavePlayer_play(HIHAT,NULL,volume);
        playArgs.inst2 = SNARE;
        sleepForMs(halfBeat);
        pthread_create(&playThread,NULL,&wavePlayer_play,(void *)&playArgs);
        //wavePlayer_play(HIHAT,SNARE,volume);
        playArgs.inst2 = NULL; 
        sleepForMs(halfBeat);
        pthread_create(&playThread,NULL,&wavePlayer_play,(void *)&playArgs);
        //wavePlayer_play(HIHAT,NULL,volume);
        sleepForMs(halfBeat);
    }
    return NULL;
}

void* mixerPlay(void* args){
    playData_t *playData = (playData_t *) args;
    char* inst1 = playData->inst1;
    char* inst2 = playData->inst2;
    wavePlayer_play(inst1,inst2,volume);
    return NULL;
}
*/
void* playBeat(){
    
        int halfBeat = 60000 / bpm / 2 ;
        createBuffer(playbackBuffer,playbackBufferSize);
        
        /*
        playData->inst1 = HIHAT;
        playData->inst2 = BASS;
        pthread_create(&playThread1, NULL, mixerPlay, (void *)playData);
        //wavePlayer_play(HIHAT,BASS,volume);
        sleepForMs(halfBeat);
        playData->inst2 = NULL;
        pthread_create(&playThread2, NULL, mixerPlay, (void *)playData);
        //wavePlayer_play(HIHAT,NULL,volume);
        sleepForMs(halfBeat);
        playData->inst2 = SNARE;
        pthread_create(&playThread1, NULL, mixerPlay, (void *)playData);
        //wavePlayer_play(HIHAT,SNARE,volume);
        sleepForMs(halfBeat);
        playData->inst2 = NULL;
        pthread_create(&playThread2, NULL, mixerPlay, (void *)playData);
        //wavePlayer_play(HIHAT,NULL,volume);
        sleepForMs(halfBeat);
        */
    
    return NULL;
}
