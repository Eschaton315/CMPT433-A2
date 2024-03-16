#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "udp.h"
#include "terminate.h"


//Defined for UDP
#define PORT 12345
#define REPLY_MAX_SIZE 1500
#define RECEIVED_MAX_SIZE 1024


//Commands defined
//1 = Silence, 2 = Rock, 3 = Custom Beat, 4 = Increase Volume
//5 = Decrease Volume, 6 = Increase Tempo, 7 = Decrease Tempo, 8 = Stop
#define COM_SILENCE "silence"
#define COM_ROCK "rock"
#define COM_CUSTOMBEAT "custombeat"
#define COM_CHANGEVOL "changevol"
#define COM_CHANGETEMPO "changetempo"
#define COM_STOP "stop"


static pthread_t udpThread;
static char reply[REPLY_MAX_SIZE];
static char msgPrev[RECEIVED_MAX_SIZE];
static bool firstCom = true;
static char unitBuffer[REPLY_MAX_SIZE];


//Function for listening to UDP packets
static void* UDPListen();

//Function for finding what command was requested 
//and running said command
static void RunCommand(char* command);

//Checks and find which command was sent
//1 = Silence, 2 = Rock, 3 = Custom Beat, 4 = Change Volume
//5 = Change Tempo, 6 = Stop
static int CheckCommand(char* command);


//Creates the thread for UDP Listen
void UDPThreadCreate(void){
	pthread_create(&udpThread, NULL, &UDPListen, NULL);
}

//Runs join to successfully close the thread
void UDPThreadJoin(void){
	pthread_join(udpThread, NULL);
}

//get value of firstCom Variable
bool getFirstCom(void){
	return firstCom;
	
}

//get value of firstCom Variable
void changeFirstCom(bool status){
	firstCom = status;
}

//Function for listening to UDP packets
static void *UDPListen(){
	
	
	char msg[RECEIVED_MAX_SIZE];
	msgPrev[0] = 0;
	
	//From Lecture notes set 6
	//Grabs the address, creates socket, 
	//Binds socket to port,
	//Receives data, sends reply if needed
	struct sockaddr_in sin;
	unsigned int sinLen;
	int bytesRx;
	
	//Grabbing necessary data for socket
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);	
	sin.sin_port = htons(PORT);	
	
	//binding socket
	int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
	bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

	while(getTerminateStatus() == false){
		
		//receiving packets
		sinLen = sizeof(sin);		
		bytesRx = recvfrom(socket_descriptor, msg, RECEIVED_MAX_SIZE, 0, (struct sockaddr *) &sin, &sinLen);
		
		msg[bytesRx] = 0;
		
		RunCommand(msg);
		
		//send reply if necessary
		if(strlen(reply) > 0){
			sinLen = sizeof(sin);
			sendto(socket_descriptor, reply, strlen(reply), 0, (struct sockaddr *) &sin, sinLen);
			
		} 
	}
	
	close(socket_descriptor);
	return NULL;
}

//Checking message to see what command was sent
//1 = Silence, 2 = Rock, 3 = Custom Beat, 4 = Change Volume
//5 = Change Tempo, 6 = Stop
static int CheckCommand(char* command){
	
	
	//Compare command to message to return what command was sent
	if(strncmp(command, COM_SILENCE, strlen(COM_SILENCE)) == 0){
		return 1;
	}
	
	if(strncmp(command, COM_ROCK, strlen(COM_ROCK)) == 0){
		return 2;
	}
	
	if(strncmp(command, COM_CUSTOMBEAT, strlen(COM_CUSTOMBEAT)) == 0){
		return 3;
	}
	
	if(strncmp(command, COM_CHANGEVOL, strlen(COM_CHANGEVOL)) == 0){
		return 4;
	}
	
	if(strncmp(command, COM_CHANGETEMPO, strlen(COM_CHANGETEMPO)) == 0){
		return 5;
	}	
	
	if(strncmp(command, COM_STOP, strlen(COM_STOP)) == 0){
		return 6;
	}	
	//if none of the strings are equal, then it is invalid
	return -1;
}

//Check the message and run the command in the message
static void RunCommand(char* command){
	
	reply[0] = 0;	
	int commandCode = CheckCommand(command);
	//Will place the necessary data in reply arr
	//Based on every command
	switch (commandCode) {		
		//Silence
		case 1:
						
			break;
		
		//Rock
		case 2:
			
			break;
			
		//Custom Beat
		case 3:
			
			break;
			
		//Change Volume
		case 4: ;
			
			strncpy(unitBuffer, command + (strlen(COM_CHANGEVOL) - 1), 3);
			
			//run atof() to convert unitBuffer to volume and use
			
			
			break;
			
		//Change Tempo
		case 5: ;
			
			strncpy(unitBuffer, command + (strlen(COM_CHANGEVOL) - 1), 3);
			
			//run atoi() to convert unitBuffer to int and use
			
			break;
			
		//STOP
		case 6:
			changeTerminateStatus(true);
			
			break;
	}
	
	
	
}

