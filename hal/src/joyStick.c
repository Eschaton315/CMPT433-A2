#include "hal/joyStick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define STICK_UP "/sys/class/gpio/gpio26/value"
#define STICK_DOWN "/sys/class/gpio/gpio46/value"
#define STICK_LEFT "/sys/class/gpio/gpio65/value"
#define STICK_RIGHT "/sys/class/gpio/gpio47/value"


//runCommand function taken from assignment page
void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
    if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

//configure pin to GPIO
void configPin(){
runCommand("config-pin p8.14 gpio");
runCommand("config-pin p8.15 gpio");
runCommand("config-pin p8.16 gpio");
runCommand("config-pin p8.18 gpio");
}

//Boolean of whether a joystick of designated path is pressed

bool joystickPressed(char *path){

FILE *pFile = fopen(path, "r");
if (pFile == NULL) {
printf("ERROR: Unable to open file (%s) for read\n", path);
return false;
}
// Read string (line)

const int MAX_LENGTH = 1024;
char buff[MAX_LENGTH];
fgets(buff, MAX_LENGTH, pFile);
// Close
fclose(pFile);

int value = atoi(buff);

if(value==0){
    return true;
}else{
    return false;
}


}

//returns which direction the joystick is pressed

int isJoystickPressed(){
    if(joystickPressed(STICK_UP)){
        return 1;
    }else if(joystickPressed(STICK_DOWN)){
        return 2;
    }else if(joystickPressed(STICK_LEFT)||joystickPressed(STICK_RIGHT)){
        return 3;
    }else{
        return 0;
    }
}

