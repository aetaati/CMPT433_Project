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
#include "lcd_display.h"
#include "bluetooth.h"
#include "audio_player.h"
#include "gpio.h"
#include "joystick.h"
#include "menuManager.h"
#include "pot.h"


#define SONG "som-liveletlive.wav"


int main(int argc, char const *argv[])
{
    /////////////////////////////////////// FOR TESTING ///////////////////////////////////////

    LCD_display_Init();

    LCD_display_ShowText("Hello World! My name is Amir.", true);

    Sleep_ms(5000);

    LCD_display_Cleanup();

    ////////////////////////////////////////////////////////////////////////////////////////////

    AudioPlayer_init();
    Potentiometer_init();


    
    
    
    wavedata_t song;
    AudioPlayer_readWaveFileIntoMemory(SONG , &song);
    AudioPlayer_playWAV(&song);
    //sleep(100);

    // Following code should be moved to menu manager
    // corresponding to the bluetooth option
    
    /*** UPDATE: CODE ADDED TO THE MENU MANAGER*/
    int selection;
    inquiry_info* devices;
    char input[15] = {0};
    devices = malloc(BT_MAX_DEV_RSP * sizeof(inquiry_info));
    int num_scanned = Bluetooth_scan(devices, BT_MAX_DEV_RSP);
    Bluetooth_displayDevices(devices, num_scanned);

    printf("Choose a device to connect to\n> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        fprintf(stderr, "error reading from stdin");
    }
    sscanf(input, "%d", &selection);



    printf("connecting to device...\n");
    if (Bluetooth_connect(&(devices + selection)->bdaddr) != 0)
    {
        printf("error connecting to device\n");
    }
    else{
        printf("Connected!\n");
    }


    sleep(40);
    return 0;
}
