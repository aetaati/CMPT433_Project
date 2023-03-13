/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: The main program to start and terminate all the modules
Author: Amirhossein Etaati
Date: 2023-03-04
*/


#include <stdbool.h>

#include "sleep.h"
// #include "lcd_display.h"
// #include "gpio.h"
#include <stdlib.h>
#include "audioMixer.h"

#define AUDIO_FILE "wave-files/Wild Ones (feat. Sia).wav"

int main(int argc, char const *argv[])
{
    /////////////////////////////////////// FOR TESTING ///////////////////////////////////////

    //LCD_display_Init();
    AudioMixer_init();

    //LCD_display_ShowText("Hello World! My name is Amir.", true);
    wavedata_t* p_data = malloc(sizeof(* p_data));
    AudioMixer_readWaveFileIntoMemory(AUDIO_FILE,p_data);

    AudioMixer_queueSound(p_data);    

    Sleep_ms(50000);
    
    AudioMixer_cleanup();
    //LCD_display_Cleanup();
    
    ////////////////////////////////////////////////////////////////////////////////////////////

    return 0;
}
