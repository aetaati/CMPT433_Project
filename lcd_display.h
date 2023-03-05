#if !defined(LCD_DISPLAY_H)
#define LCD_DISPLAY_H

// Exports the pins for LCD and initializes the LCD
void LCD_displayInit();

// Displays "text" on the screen
void LCD_displayText(char *text, int size);

// Turns the LCD off
void LCD_displayCleanup();


#endif // LCD_DISPLAY_H
