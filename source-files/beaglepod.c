/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: The main program to start and terminate all the modules
Author: Amirhossein Etaati
Date: 2023-03-04
*/

#include <stdbool.h>

#include "sleep.h"
#include "lcd_display.h"
#include "bluetooth.h"
#include "gpio.h"
#include "audio-mixer.h"

int main(int argc, char const *argv[])
{
    /////////////////////////////////////// FOR TESTING ///////////////////////////////////////

    /*LCD_display_Init();

    LCD_display_ShowText("Hello World! My name is Amir.", true);

    Sleep_ms(5000);

    LCD_display_Cleanup();*/

    ////////////////////////////////////////////////////////////////////////////////////////////

    Bluetooth_init();
   

    Sleep_ms(10000000);
    return 0;
}
