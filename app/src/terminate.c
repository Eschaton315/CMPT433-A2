#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "terminate.h"

static bool terminate = false;

void changeTerminateStatus(bool status){
	terminate = status;
	
}

bool getTerminateStatus(void){
	return terminate;
}