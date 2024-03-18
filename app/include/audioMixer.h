#ifndef _AUDIOMIXER_H_
#define _AUDIOMIXER_H_
#include "hal/wavePlayer.h"

void audioMixer_init();

void audioMixer_setVol(float newVol);

float audioMixer_getVol();

void audioMixer_setBpm(int newBpm);

int audioMixer_getBpm();

void audioMixer_setBeat(int newBeatNum);

int audioMixer_getBeat();

void audioMixer_cleanup();

#endif