#if !defined(LCD_DISPLAY_H)
#define LCD_DISPLAY_H

// Exports the pins for LCD and initializes the LCD
void LCD_display_Init();

// Displays "text" on the screen
void LCD_display_ShowText(char *text, int size);

// Turns the LCD off
void LCD_display_Cleanup();

#endif // LCD_DISPLAY_H
