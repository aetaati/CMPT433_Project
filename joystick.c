/*
Author: Amirhossein Etaati, Mehdi Esmaeilzadeh
Date: 2023-02-25
Subject: Implementation of the Joystick module to get the current accelerometer action
*/



#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "joystick.h"



#include <alsa/asoundlib.h>

// static enum Current_songPlaying song_playing = BASE_DRUM_SOUND;

static void assertDirectionOk(enum eJoystickDirections direction);

struct DirectionInfo
{
    const char *name;
    const int portNumber;
};

// Data on all different directions which we support
const struct DirectionInfo directions[] = {
        {"Up",     26,},
        {"Down",   46,},
        {"Left",   65,},
        {"Right",  47,},
        {"Center", 27,},
};

void Joystick_init()
{
    int length = strlen(GPIO_FILE_DIRECTION) + 1;
    for (int i = 0; i < (JOYSTICK_MAX_NUMBER_DIRECTIONS - 1); i++)
    {
        // printf("... todo: actually open file for GPIO #%d\n", directions[i].portNumber);
        char filepath[length];
        sprintf(filepath, GPIO_FILE_DIRECTION, directions[i].portNumber);
        FILE *pFile = fopen(filepath, "w");
        if (pFile == NULL)
        {
            printf("ERROR: Unable to open export file.\n");
            exit(1);
        }
        // Write to data to the file using fprintf():
        fprintf(pFile, "in");
        // Close the file using fclose():
        fclose(pFile);
    }
}

void Joystick_cleanup()
{ 
    // Nothing to clean up 
}

bool Joystick_isDirectionPressed(enum eJoystickDirections direction)
{
    assertDirectionOk(direction);

    int length = strlen(GPIO_FILE_VALUE) + 1;
    char filepath[length];
    sprintf(filepath, GPIO_FILE_VALUE, directions[direction].portNumber);

    FILE *pFile = fopen(filepath, "r");
    if (pFile == NULL)
    {
        printf("ERROR: Unable to open file (%s) for read\n", filepath);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    fclose(pFile);
    if (buff[0] == '1')
    {
        return false;
    }
    else
    {
        return true;
    }
}

enum eJoystickDirections Joystick_process_direction()
{
    if (Joystick_isDirectionPressed(JOYSTICK_UP))
    {
        return JOYSTICK_UP;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_DOWN))
    {
        return JOYSTICK_DOWN;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_RIGHT))
    {
        return JOYSTICK_RIGHT;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_LEFT))
    {
         return JOYSTICK_LEFT;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_CENTER))
    {       
        return JOYSTICK_CENTER;

    }
    else {
        return JOYSTICK_NONE;
    }

}

const char *Joystick_getDirectionName(enum eJoystickDirections direction)
{
    assertDirectionOk(direction);

    return directions[direction].name;
}

static void assertDirectionOk(enum eJoystickDirections direction)
{
    assert(direction >= 0 && direction < (JOYSTICK_MAX_NUMBER_DIRECTIONS - 1));
}