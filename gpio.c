/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Definitions of the LCD module functions to manage text display
Author: Amirhossein Etaati
Date: 2023-03-04
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#include "gpio.h"
#include "sleep_ms.h"

// To check if the module functions' caller has called init first
static bool is_module_initialized = false;

// Location of the file to export pins
#define EXPORT_PATH "/sys/class/gpio/export"

// Maximum length of a file path
    // Used for exporting pins and setting their direction and value 
#define MAX_PATH_LEN 1024

// Pin information
#define DIGIT_GPIO_PATH "/sys/class/gpio/gpio" 
#define DIR_PATH "/direction"





/****************************** Private/Static Functions ******************************/

// Helper function to run linux command
// Note: adapted from assignment one description
static void runCommand(const char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        }
        // printf("--> %s", buffer); // Uncomment for debugging
        // Get the exit code from the pipe; non-zero is an error:
        int exitCode = WEXITSTATUS(pclose(pipe));
        if (exitCode != 0) {
            perror("Unable to execute command:");
            printf(" command: %s\n", command);
            printf(" exit code: %d\n", exitCode);
        }
}

// Helper function to enable buses for i2c
// Enables linux support for the bus "/dev/i2c-1" by configuring the data and clock pins (SDA and SCL)
static void enable_buses(const char **buses, int bus_size)
{
    for (size_t i = 0; i < bus_size; i++) {
        runCommand(buses[i]);
    }    
}

// Helper function to export "pin_size" of pins in "pins"
static void export_pins(const int *pins, int pin_size)
{
    for (size_t i = 0; i < pin_size; i++) {
         // concatenate to get pin's file location
        char direction_path[MAX_PATH_LEN];
        sprintf(direction_path, "%s%d", DIGIT_GPIO_PATH, pins[i]);

        // check to see if the pin is already exported
        if (access(direction_path, F_OK) == 0) {
            // file exists - don't need to import it
            continue;
        }

        // open pin file for write access
        FILE *exportFile = fopen(EXPORT_PATH, "w");
        if (exportFile == NULL) {
            printf("Led_display ERROR: Unable to open export file (export_pins). Pin#=%d\n", pins[i]);
            exit(1);
        }

        fprintf(exportFile, "%d", pins[i]);

        fclose(exportFile);
    }

    // wait for ~300ms before use
    Sleep_ms(300);
}


// Helper function to set pins' directions to "pin_dir"
static void set_pin_direction(const int *pins, int pin_size, const char *pin_dir)
{
    for (size_t i = 0; i < pin_size; i++) {
        // concatenate to get pin's direction file location
        char direction_path[MAX_PATH_LEN];
        sprintf(direction_path, "%s%d%s", DIGIT_GPIO_PATH, pins[i], DIR_PATH);

        // open pin file for write access
        FILE *exportFile = fopen(direction_path, "w");
        if (exportFile == NULL) {
            printf("Led_display - ERROR: Unable to open export file (set_direction). %d\n", pins[i]);
            exit(1);
        }

        // write DIR to file
        fprintf(exportFile, "%s", pin_dir);

        fclose(exportFile);
    }
}



/****************************** Public/Module Functions ******************************/

void GPIO_init(const char **buses_config_commands, int bus_size, const int *pins, int pin_size, char *pin_dir) {
    // handle errors
    if (buses_config_commands != NULL && bus_size != 0) {
        // enable linux support for the required buses
        enable_buses(buses_config_commands, bus_size);
    }
    if (pins != NULL || pin_size != 0) {
        // export the pins
        export_pins(pins, pin_size);
    }
    if (pin_dir != NULL){
        // set pin directions
        set_pin_direction(pins, pin_size, pin_dir);
    }
    
    is_module_initialized = true;
}

void GPIO_cleanup(int *pins, int size) {
    
    assert(is_module_initialized);
    // does nothing
}
