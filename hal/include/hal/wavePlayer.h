#ifndef _WAVEPLAYER_H_
#define _WAVEPLAYER_H_
#include <alsa/asoundlib.h>

typedef struct
{
	int numSamples;
	short *pData;
} wavedata_t;

void configPinI2C();

//void* wavePlayer_play(void *arguments /*char* path,char* path2, float volume*/);

void wavePlayer_play( char* path, float volume);

void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);

snd_pcm_t *Audio_openDevice();

void Audio_setVolume(wavedata_t *sampleData, float volume);

void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);

void wavePlayer_cleanup(snd_pcm_t* handle);

#endif