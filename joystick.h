/*
Author: Mehdi Esmaeilzadeh
Date: 2023-03-10
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
// Initialize the Joystick module
void Joystick_init();
// Remove any dynamic memory or open file
void Joystick_cleanup();
// Returns the direction hit by the user
enum eJoystickDirections Joystick_process_direction();
// Is the given direction pressed by the user
bool Joystick_isDirectionPressed(enum eJoystickDirections direction);
// Returns "Up", "Down", "Left", "Right" as a char *
const char* Joystick_getDirectionName(enum eJoystickDirections direction);



#endif //INC_05_C_JOYSTICK_H