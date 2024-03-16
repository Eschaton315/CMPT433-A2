#ifndef _WAVEPLAYER_H_
#define _WAVEPLAYER_H_
#include <alsa/asoundlib.h>

void configPinI2C();

//void* wavePlayer_play(void *arguments /*char* path,char* path2, float volume*/);

void wavePlayer_play( char* path, float volume);

Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);

snd_pcm_t *Audio_openDevice();

//void wavePlayer_cleanup(snd_pcm_t* handle);

#endif