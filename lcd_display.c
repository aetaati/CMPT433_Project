/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Implementation of the LCD module to handle text display
Author: Amirhossein Etaati
Date: 2023-03-04
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "lcd_display.h"
#include "gpio.h"

// To check if the module functions' caller has called init first
static bool is_module_initialized = false;

// Macros/Constants for the commands to enable linux support for the bus "/dev/i2c-1"
#define SDA " P9_18 " // data pin
#define SCL " P9_17 " // clock pin
#define CONFIG_PIN(pin) "config-pin" pin "i2c"
#define BUSES_CONFIG_COUNT 2
static const char *buses_config_commands[BUSES_CONFIG_COUNT] = {CONFIG_PIN(SDA), CONFIG_PIN(SDA)}; // config commands to enable buses 

// Macros for the pins associated with the LED display that need to be exported
#define PIN_COUNT 6
static const int pins[] = {48, 49, 66, 67, 68, 69};
#define VAL_PATH "/value"
#define DIR "out"

/****************************** Private/Static Functions ******************************/


/****************************** Public/Module Functions ******************************/

void LCD_display_Init()
{
    // GPIO initialization for the LCD - enable the busses and configure the pins
    GPIO_init(buses_config_commands, BUSES_CONFIG_COUNT, pins, PIN_COUNT, DIR);

    // Set the LCD display registers to the initial state

    is_module_initialized = true;
}

void LCD_display_ShowText(char *text, int size)
{
    assert(is_module_initialized);
    // TODO
}

void LCD_display_Cleanup()
{
    assert(is_module_initialized);    

    GPIO_cleanup();

    is_module_initialized = false;
}



/****************************** For testing only ******************************/
int main(int argc, char const *argv[])
{
    LCD_display_Init();


    LCD_display_Cleanup();
    return 0;
}

