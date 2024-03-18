#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h> 
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include "hal/acc.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_X0 0x32
#define REG_X1 0x33
#define REG_Y0 0x34
#define REG_Y1 0x35
#define REG_Z0 0x36
#define REG_Z1 0x37



static int initI2cBus(char* bus, int address);
static int readI2cReg(int i2cFileDesc, unsigned char regAddr);
static void *accListener();

pthread_t accThread;
static int i2cFileDesc;
static int val1;
static int val2;
static double x = -999999;
static double xPrev = -999999;
static double y = -999999;
static double yPrev = -999999;
static double z = -999999;
static double zPrev = -999999;
static int regVal;
static bool statusPlayHiHat = false;
static bool statusPlaySnare = false;
static bool statusPlayBase = false;
static bool statusHold;
static bool EndListen = false;


//Initialize the accelerometer
void acc_init(){
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    pthread_create(&accThread, NULL, &accListener,NULL);
    return;
}

//Clean up the accelerometer
void accListener_cleanup(){
    EndListen = true;
	close(i2cFileDesc);
    pthread_join(accThread,NULL);

    return;
}

//runs the accelerometer
static void *accListener(){
	while(!EndListen){
		regVal = readI2cReg(i2cFileDesc, REG_X0);
		val1 = regVal;
		regVal = readI2cReg(i2cFileDesc, REG_X1);
		val2 = regVal;
		x = formatRawData(val1, val2);
		
		regVal = readI2cReg(i2cFileDesc, REG_Y0);
		val1 = regVal;
		regVal = readI2cReg(i2cFileDesc, REG_Y1);
		val2 = regVal;
		y = formatRawData(val1, val2);
		
		regVal = readI2cReg(i2cFileDesc, REG_Z0);
		val1 = regVal;
		regVal = readI2cReg(i2cFileDesc, REG_Z1);
		val2 = regVal;
		z = formatRawData(val1, val2);
		
		playSounds();
		setPrevXYZ();
		
	}
    return NULL;
}

//Set values of prevX, prevY, and prevZ
void setPrevXYZ(){
	xPrev = x;
	yPrev = y;
	zPrev = z;
	
}

//Play sounds from the accelerometer based on movement
void playSounds(){
	if(fabs(x - xPrev) > 7000){
		//play hi-hat
		statusPlayHiHat = true;
		
	}
	
	if(fabs(y - yPrev) > 7000){
		//play snare
		statusPlaySnare = true;
		
	}
	
	if(fabs(z - zPrev) > 13000){
		//play base
		statusPlayBase = true;
		
	}
	
}

bool playHiHat(){
	statusHold = statusPlayHiHat;
	statusPlayHiHat = false;
	return statusHold;
	
}

bool playSnare(){
	statusHold = statusPlayHiHat;
	statusPlaySnare = false;
	return statusHold;
	
}

bool playBase(){
	statusHold = statusPlayHiHat;
	statusPlayBase = false;
	return statusHold;
	
}

//format the data from the 2 registers to the raw data number
double formatRawData( int data0, int data1) { 
	static double maxResolution = 256.00; 
	data1 = data1 << 8; int dataOut = data0 + data1; 
	double data = (double) dataOut / maxResolution; 
	return data;
}

//initialize bus
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}



//read the register
static int readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("Unable to write i2c register.");
        exit(-1);
    }
    unsigned char buff[6];
    // Now read the value and return it
    res = read(i2cFileDesc, &buff, 6 * sizeof(unsigned char));
    if (res != sizeof(buff)) {
        perror("I2C: Unable to read from i2c register");
        exit(-1);
    }

      // Convert buff to int here
    int intValue = 0;
    for (int i = 0; i < 6; ++i) {
        intValue |= (buff[i] << (i * 8)); // Assuming each byte of buff corresponds to one byte of int
    }

    return intValue;
}