#ifndef _ACC_H_
#define _ACC_H_

//Initialize the accelerometer
void acc_init();

//Clean up the accelerometer
void accListener_cleanup();

//Play sounds from the accelerometer based on movement
void playSounds();

//Set values of prevX,Y, and Z
void setPrevXYZ();

//format the data from the 2 registers to the raw data number
double formatRawData( int data0, int data1) ;

//Check if Hi Hat should be played
bool playHiHat();

//Check if snare should be played
bool playSnare();

//Check if base should be played
bool playBase();


#endif