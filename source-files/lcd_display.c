/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Definition of the LCD module to handle text display
Author: Amirhossein Etaati
Date: 2023-03-04
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h> // uint8_t
#include <string.h> // strlen()

#include "lcd_display.h"
#include "gpio.h"
#include "sleep.h"

#define MAX_ROW_CHARS_DISPLAY 16
#define MAX_TOTAL_CHARS_DISPLAY 32

// To check if the module functions' caller has called init first
static bool is_module_initialized = false;

// Macros/Constants for the commands to enable linux support for the bus "/dev/i2c-1"
#define SDA " P9_18 " // data pin
#define SCL " P9_17 " // clock pin
#define CONFIG_PIN(pin) "config-pin" pin "i2c"
#define BUSES_CONFIG_COUNT 2
static const char *buses_config_commands[BUSES_CONFIG_COUNT] = {CONFIG_PIN(SDA), CONFIG_PIN(SDA)}; // config commands to enable buses 

// Macros for the pins associated with the LED display that need to be exported
#define D4_GPIO_NUMBER 66
#define D5_GPIO_NUMBER 69
#define D6_GPIO_NUMBER 115
#define D7_GPIO_NUMBER 48
#define RS_GPIO_NUMBER 68   // to set what message type is sent to the LCD micro controller: 0 (ASCI) or 1 (clear display, move cursor)
#define E_GPIO_NUMBER  67   // to inform the LCD micro controller that there is data in D4 - D7 to be read

#define PIN_COUNT 6
static const int pins_[] = {D7_GPIO_NUMBER, D6_GPIO_NUMBER, D4_GPIO_NUMBER, E_GPIO_NUMBER, RS_GPIO_NUMBER, D5_GPIO_NUMBER};
#define DIR "out"

/****************************** Private/Static Functions ******************************/

// Helper Function: Flashes the E pin high to low to have the LCD consume the data
// Note: Adapted from the 16x2CharacterLCDThroughGPIO student guide
    // URL: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/16x2CharacterLCDThroughGPIO.pdf
static void pulseEnable() {
    GPIO_SetPinValue(E_GPIO_NUMBER, "1");
    Sleep_ns(0, 1000000);

    GPIO_SetPinValue(E_GPIO_NUMBER, "0");
    Sleep_ns(0, 1000000);
}

// Helper Function: Sets all pins except RS_GPIO_NUMBER to zero
static void setDataPinsToZero(const int *pins, int pin_count)
{
    for (size_t i = 0; i < pin_count; i++) {
        if(pins[i] != RS_GPIO_NUMBER) {
            GPIO_SetPinValue(pins[i], "0");
        }
    }
}

// Helper Function: Separates the four bits of value and writes them to D4, D5, D6, and D7 from least significant to most significant bits
// Note: Adapted from the 16x2CharacterLCDThroughGPIO student guide
    // URL: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/16x2CharacterLCDThroughGPIO.pdf
static void write4Bits(uint8_t value)
{
    char strBit[2] = {0 , 0};
    strBit[1] = '\0'; 
    strBit[0] = (value & 0x01 ? 1 : 0) + '0';
	GPIO_SetPinValue(D4_GPIO_NUMBER, strBit);

    strBit[0] = ((value >> 1) & 0x01 ? 1 : 0) + '0';
    GPIO_SetPinValue(D5_GPIO_NUMBER, strBit);

    strBit[0] = ((value >> 2) & 0x01 ? 1 : 0) + '0';
    GPIO_SetPinValue(D6_GPIO_NUMBER, strBit);

    strBit[0] = ((value >> 3) & 0x01 ? 1 : 0) + '0';
    GPIO_SetPinValue(D7_GPIO_NUMBER, strBit);
    
	pulseEnable();
}

// Helper Function: Resets LCD
static void reset_LCD(void) 
{
    // the LCD recognizes these three consecutive command as a reset

    // special function set 1
	write4Bits(0x03); // 0011
    Sleep_ns(0, 5000000); // 5 ms

	// special function set 2
	write4Bits(0x03); // 0011
    Sleep_ns(0, 128000); // 128 us
    
	// special function set 3
	write4Bits(0x03); // 0011
    Sleep_ns(0, 128000); // 128 us
}

// Helper Function: Sets the LCD screen to it's initial state
// Note: Adapted from the 16x2CharacterLCDThroughGPIO student guide
    // URL: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/16x2CharacterLCDThroughGPIO.pdf
static void displayInitialState(void) 
{
    // set the value of all data pins to 0
    setDataPinsToZero(pins_, PIN_COUNT);

    // set the mode to command
    GPIO_SetPinValue(RS_GPIO_NUMBER, "0");

    // rest the LCD 
    reset_LCD();

	// sets to 4-bit operation.
	write4Bits(0x2); /* 0010 */
	Sleep_ns(0, 1000000); // 1 ms

	// sets to 4-bit operation. Sets 2-line display. Selects 5x8 dot character font.
	write4Bits(0x2); /* 0010 - We can alternatively write 0000 here for 8-bit operation. */
	write4Bits(0x8); /* 1000 - We can alternatively write 1100 here for 5x10 dot font. */
    Sleep_ns(0, 128000); // 128 us

	// display ON/OFF control.
	write4Bits(0x0); /* 0000 */
	write4Bits(0x8); /* 1000 */
	Sleep_ns(0, 128000); // 128 us

    // clear the display.
    write4Bits(0x0); /* 0000 */
	write4Bits(0x1); /* 0001 */
	Sleep_ns(0, 64000); // 64 us

	// sets mode to increment cursor position by 1 and shift right when writing to display.
	write4Bits(0x0); /* 0000 */
	write4Bits(0x6); /* 0110 */
	Sleep_ns(0, 128000); // 128 us

	// turns on display. This corresponds to the instruction 0000 1100 in binary.
	// to be able to see the cursor, use 0000 1110.
	// to enable cursor blinking, use 0000 1111.
	write4Bits(0x0); /* 0000 */
	write4Bits(0xC); /* 1100 */ // No cursor 
	Sleep_ns(0, 64000); // 64 us

	// pull RS up to write data.
	GPIO_SetPinValue(RS_GPIO_NUMBER, "1");
}

// Helper Function: Writes "msg" on the LCD display
static void writeMessage(char* msg, int start_idx)
{
    int end_idx = (strlen(msg) < MAX_TOTAL_CHARS_DISPLAY) ? strlen(msg) : MAX_TOTAL_CHARS_DISPLAY;
    for (int i = start_idx; i < end_idx; i++) {
        char c = msg[i];
        
        unsigned int upper_bits = (c >> 4);
	    unsigned int lower_bits = c & 0xF;

	    write4Bits(upper_bits);
	    write4Bits(lower_bits);
    }
}

/****************************** Public/Module Functions ******************************/

void LCD_display_Init()
{
    // GPIO initialization for the LCD - enable the busses, configure the pins, and sets direction
    GPIO_init(buses_config_commands, BUSES_CONFIG_COUNT, pins_, PIN_COUNT, DIR);

    // Set the LCD display registers to the initial state
    displayInitialState();

    is_module_initialized = true;
}

void LCD_display_ShowText(char *text, bool slide)
{
    assert(is_module_initialized);

    if(strlen(text) > MAX_TOTAL_CHARS_DISPLAY) {
        return;
    }

    if(slide && (strlen(text) > MAX_ROW_CHARS_DISPLAY)) {
        // Need to slide text in one row
        if(strlen(text) > MAX_ROW_CHARS_DISPLAY) {
            
            for (size_t i = 0; i < strlen(text); i++) {
                writeMessage(text, i);
                
                Sleep_ms(500);
                displayInitialState();
            }
        }
    }
    else if (!slide && (strlen(text) > MAX_ROW_CHARS_DISPLAY)) {
        // Need to display in two rows
        if(strlen(text) > MAX_ROW_CHARS_DISPLAY) {
            // Write to both rows
            writeMessage(text, 0);

            // With trial and error, I realized that I need to write 40 characters in one row before going to the next
            // TODO: WORKS BUT NEED TO FIGURE OUT THE CORRECT WAY OF DOING IT
            for (size_t i = 0; i < (40 - strlen(text)); i++) {
                writeMessage(" ", 0);
            }
            
            writeMessage(text, MAX_ROW_CHARS_DISPLAY);
            Sleep_ms(500);
        }
    }
    else {
        // display the complete text
        writeMessage(text, 0);
        Sleep_ms(500);
    }

    if(slide) {
        // at the end, display the cropped text with ... if sliding was done
        char *text_with_dot = calloc(MAX_ROW_CHARS_DISPLAY, sizeof(char));

        snprintf(text_with_dot, MAX_ROW_CHARS_DISPLAY, "%s", text);
        text_with_dot[MAX_ROW_CHARS_DISPLAY - 4] = '.';
        text_with_dot[MAX_ROW_CHARS_DISPLAY - 3] = '.';
        text_with_dot[MAX_ROW_CHARS_DISPLAY - 2] = '.';
        
        writeMessage(text_with_dot, 0);
        free(text_with_dot);

        Sleep_ms(500);
    }
}

void LCD_display_Cleanup()
{
    assert(is_module_initialized);    

    GPIO_cleanup();

    displayInitialState();

    is_module_initialized = false;
}