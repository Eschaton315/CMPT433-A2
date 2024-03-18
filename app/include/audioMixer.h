#ifndef _AUDIOMIXER_H_
#define _AUDIOMIXER_H_
#include "hal/wavePlayer.h"

void audioMixer_init();

void audioMixer_setVol(float newVol);

void audioMixer_setbpm(int newBpm);

void audioMixer_setBeat(int newBeatNum);

void audioMixer_cleanup();

#endif