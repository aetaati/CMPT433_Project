/*
Purpose: Declaration of the GPIO module functions to manage bus and pin configuration
*/

#if !defined(EXPORT_PINT_H)
#define EXPORT_PINT_H

// Enables the busses and exports "pin_size" number of "*pins" if not already exported,
    // then sets the direction to "pin_dir"
// Node: "buses" should be an array of null-terminated config-pin commands
void GPIO_init(const char **buses_config_commands, int bus_size, const int *pins, int pin_size, char *pin_dir);

// Set the direction of the pin associated to "pin_number" to "direction"
void GPIO_SetPinDirection(int pin_number, const char *direction);

// Set the "value" of the file associated to "pin_number"
void GPIO_SetPinValue(int pin_number, const char *value);

// This function does nothing - no need to unable bus or un-export the pins
void GPIO_cleanup();



#endif // EXPORT_PINT_H
