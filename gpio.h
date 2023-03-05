/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Declaration of the LCD module functions to manage text display
Author: Amirhossein Etaati
Date: 2023-03-04
*/

#if !defined(EXPORT_PINT_H)
#define EXPORT_PINT_H

// Enables the busses and exports "pin_size" number of "*pins" if not already exported,
    // then sets the direction to "pin_dir"
// Node: "buses" should be an array of null-terminated config-pin commands
void GPIO_init(const char **buses_config_commands, int bus_size, const int *pins, int pin_size, char *pin_dir);


// Write "value" to the file located at "file_path"
void GPIO_writeFile(const char *file_path, char *value);


// This function does nothing - no need to unable bus or un-export the pins
void GPIO_cleanup();



#endif // EXPORT_PINT_H
