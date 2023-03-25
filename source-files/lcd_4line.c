#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>

#include "lcd_4line.h"
#include "sleep.h"

#define I2C_BUS "/dev/i2c-1"
#define I2C_ADDR 0x27 // I2C slave address for the LCD module



// GLOBALS
static int i2cFd;

// Private Functions
void setLineNum(LCD_LINE_NUM line_num);
void i2c_send_byte(unsigned char data);

void LCD_init(void)
{
    if ((i2cFd = open(I2C_BUS, O_RDWR)) < 0)
    {
        printf("Error failed to open I2C bus [%s].\n", I2C_BUS);
        exit(-1);
    }
    // set the I2C slave address for all subsequent I2C device transfers
    if (ioctl(i2cFd, I2C_SLAVE, I2C_ADDR) < 0)
    {
        printf("Error failed to set I2C address [%0x].\n", I2C_ADDR);
        exit(-1);
    }

    /* -------------------------------------------------------------------- *
     * Initialize the display, using the 4-bit mode initialization sequence *
     * -------------------------------------------------------------------- */

    Sleep_ns(0 , 1500000);             // wait 15msec
    i2c_send_byte(0b00110100); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=1
    i2c_send_byte(0b00110000); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=0
    Sleep_ns(0 , 4100000);               // wait 4.1msec
    i2c_send_byte(0b00110100); //
    i2c_send_byte(0b00110000); // same
    Sleep_ns(0 , 100000);                // wait 100usec
    i2c_send_byte(0b00110100); //
    i2c_send_byte(0b00110000); // 8-bit mode init complete
    Sleep_ns(0 , 4100000);              // wait 4.1msec
    i2c_send_byte(0b00100100); //
    i2c_send_byte(0b00100000); // switched now to 4-bit mode

    /* -------------------------------------------------------------------- *
     * 4-bit mode initialization complete. Now configuring the function set *
     * -------------------------------------------------------------------- */
    Sleep_ns(0, 40000);              // wait 40usec
    i2c_send_byte(0b00100100); //
    i2c_send_byte(0b00100000); // keep 4-bit mode
    i2c_send_byte(0b10000100); //
    i2c_send_byte(0b10000000); // D3=1 line, D2=char5x8

    LCD_setCursorDirection();

    LCD_turnOnDisplay();

    LCD_clear();
}


void LCD_cleanup(void) { close(i2cFd); }

void LCD_writeStringAtLine(char *string, LCD_LINE_NUM line_num)
{
    setLineNum(line_num);
    if (line_num == LCD_LINE1)
    {
        LCD_writeChar(LCD_RIGHT_ARROW);
    }
    LCD_writeString(string);
}

void LCD_writeChar(unsigned char character)
{
    unsigned char full = 0x00;
    unsigned char high = character & 0xF0;
    unsigned char low = (character & 0x0F) << 4;
    full = full | high;
    full = full | 0x05;

    i2c_send_byte(full);
    printf("sent: %0x\n", full);
    i2c_send_byte(0b00000000);

    full = 0x00;
    full = full | low;
    full = full | 0x05;

    i2c_send_byte(full);
    printf("sent: %0x\n", full);
    i2c_send_byte(0b00000000);
}

void LCD_clear(void)
{
    i2c_send_byte(0b00000100);
    i2c_send_byte(0b00000000);
    i2c_send_byte(0b00010100);
    i2c_send_byte(0b00000000);
}

void LCD_turnOnDisplay(void)
{
    Sleep_ns(0, 40000);                // wait 40usec
    i2c_send_byte(0b00000100); //
    i2c_send_byte(0b00000000); // D7-D4=0
    i2c_send_byte(0b11000100); //
    i2c_send_byte(0b11000000); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink
}

void LCD_setCursorDirection(void)
{
    i2c_send_byte(0b00000100);
    i2c_send_byte(0b00000000);
    i2c_send_byte(0b01100100);
    i2c_send_byte(0b01100000); // print left to right
}

void LCD_writeString(char *string)
{
    while (*string != '\0')
    {
        LCD_writeChar(*string);
        string++;
    }
}

void i2c_send_byte(unsigned char data)
{
    unsigned char byte[1];
    byte[0] = data;
    write(i2cFd, byte, sizeof(byte));
    /* -------------------------------------------------------------------- *
     * Below wait creates 1msec delay, needed by display to catch commands  *
     * -------------------------------------------------------------------- */
    Sleep_ns(0, 1000000);
}

void setLineNum(LCD_LINE_NUM line_num)
{

    switch (line_num)
    {
    case LCD_LINE1:
        // 0x00
        i2c_send_byte(0b00000100);
        i2c_send_byte(0b00000000);
        i2c_send_byte(0b00000100);
        i2c_send_byte(0b00000000);
        break;
    case LCD_LINE2:
        // 0x40
        i2c_send_byte(0b11000100);
        i2c_send_byte(0b00000000);
        i2c_send_byte(0b00000100);
        i2c_send_byte(0b00000000);
        break;
    case LCD_LINE3:
        // 0x14
        i2c_send_byte(0b10010100);
        i2c_send_byte(0b00000000);
        i2c_send_byte(0b01000100);
        i2c_send_byte(0b00000000);
        break;
    case LCD_LINE4:
        i2c_send_byte(0b11010100);
        i2c_send_byte(0b00000000);
        i2c_send_byte(0b01000100);
        i2c_send_byte(0b00000000);
        break;
    default:
        // invalid line
        printf("LCD_setLineNum : invalid line num\n");
        break;
    }
}