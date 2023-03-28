/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: The main program to start and terminate all the modules
Author: Amirhossein Etaati
Date: 2023-03-04
*/
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


#include "sleep.h"
#include "shutdown.h"
#include "bluetooth.h"
#include "audio_player.h"
#include "gpio.h"
#include "joystick.h"
#include "menuManager.h"
#include "volume.h"
#include "lcd_4line.h"


#define SONG "songs/som-liveletlive.wav"
#define SONG3 "songs/hunnybee.wav"
#define SONG4 "songs/moves.wav"
#define SONG5 "songs/kiss-from-rose.wav"


int main(int argc, char const *argv[])
{

    AudioPlayer_init();
    Potentiometer_init();
    MenuManager_init();

    Shutdown_init();
    Shutdown_waitForShutdown();

    MenuManager_cleanup();
    Potentiometer_cleanup();
    AudioPlayer_cleanup();





    /*printf("connecting to device...\n");
    if (Bluetooth_connect(&(scanner->devices + selection)->bdaddr) != 0)
    {
        printf("error connecting to device\n");
    }
    else{
        printf("Connected!\n");
    }*/

    return 0;
}
