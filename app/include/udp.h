#ifndef _UDP_H_
#define _UDP_H_
#include <stdbool.h>

//Creates the thread for UDP Listen
void UDPThreadCreate(void);

//Runs join to successfully close the thread
void UDPThreadJoin(void);

//get value of firstCom Variable
bool getFirstCom(void);

//get value of firstCom Variable
void changeFirstCom(bool status);

//read file content and place in reply
bool ReadFileContents(char *path);

#endif