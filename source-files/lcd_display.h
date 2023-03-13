/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Declaration of the LCD module to handle text display
Author: Amirhossein Etaati
Date: 2023-03-04
*/


#if !defined(LCD_DISPLAY_H)
#define LCD_DISPLAY_H

// Exports the pins for LCD and initializes the LCD
void LCD_display_Init();

// Displays "text" on the screen
// Note: If "text" is longer than 16 characters and "slide" is true, 
    // "text" will slide on the first row. 
    // If it's false, "text" will be displayed on both rows
// Note: "text" can be at most 32 characters long
    // TODO: correct implementation
void LCD_display_ShowText(char *text, bool slide);

// Turns the LCD off
void LCD_display_Cleanup();

#endif // LCD_DISPLAY_H
