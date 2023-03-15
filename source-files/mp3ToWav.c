#include "mp3ToWav.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define CONVERT_COMMAND "ffmpeg -y -i "

#define MAX_STR_SIZE 4096

// function to run linux command
// Note: adapted from assignment one description
static void runCommand(char* command)
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


bool isModuleInitialize = false;

void mp3ToWave_init(void) 
{
    isModuleInitialize = true;
}

void mp3ToWave_convert(char *mp3_path, char *wav_path)
{
    assert(isModuleInitialize);

    char *str_command = malloc(sizeof(char)*MAX_STR_SIZE);
    strcpy(str_command, CONVERT_COMMAND);

    strcat(str_command, mp3_path);

    strcat(str_command, " ");

    strcat(str_command, wav_path);

    strcat(str_command, " 2> /dev/null");

    runCommand(str_command);

    free(str_command);
}

void mp3ToWave_cleanup(void) 
{
    assert(isModuleInitialize);
    isModuleInitialize = false;
}
