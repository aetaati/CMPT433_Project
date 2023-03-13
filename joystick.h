/*
Author: Amirhossein Etaati, Mehdi Esmaeilzadeh
Date: 2023-02-25
Subject: Definition of the Joystick module to get the current accelerometer action
*/


#ifndef INC_05_C_JOYSTICK_H
#define INC_05_C_JOYSTICK_H

/* The  Joystick Interface: The Joystick.h and Joystick.c is responsible for setting up joystick and handling 
the joystick movement when pressed.  */

#define GPIO_FILE_DIRECTION "/sys/class/gpio/gpio%d/direction"
#define GPIO_FILE_VALUE "/sys/class/gpio/gpio%d/value"

enum eJoystickDirections
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_CENTER,
    JOYSTICK_NONE,
    JOYSTICK_MAX_NUMBER_DIRECTIONS      // Get the number of directions via the enum
};

void Joystick_init();
void Joystick_cleanup();
enum eJoystickDirections Joystick_process_direction();
bool Joystick_isDirectionPressed(enum eJoystickDirections direction);
const char* Joystick_getDirectionName(enum eJoystickDirections direction);



#endif //INC_05_C_JOYSTICK_H