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

    /*AudioPlayer_init();
    Potentiometer_init();
    MenuManager_init();

    Shutdown_init();
    Shutdown_waitForShutdown();

    MenuManager_cleanup();
    Potentiometer_cleanup();
    AudioPlayer_cleanup();*/



    //LCD_shiftDisplay(LCD_LINE1, LCD_RIGHT);

    
    
    //sleep(100);

    // Following code should be moved to menu manager
    // corresponding to the bluetooth option
    
    /*** UPDATE: CODE ADDED TO THE MENU MANAGER*/
    int selection;
    char input[15] = {0};
    bluetooth_scan_t* scanner = malloc(sizeof(bluetooth_scan_t));
    Bluetooth_scan(scanner);


    Bluetooth_displayDevices(scanner->devices, scanner->num_devices);

    printf("Choose a device to connect to\n> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "error reading from stdin");
    }
    sscanf(input, "%d", &selection);



    printf("connecting to device...\n");
    if (Bluetooth_connect(&(scanner->devices + selection)->bdaddr) != 0)
    {
        printf("error connecting to device\n");
    }
    else{
        printf("Connected!\n");
    }


    sleep(200);
    return 0;
}
