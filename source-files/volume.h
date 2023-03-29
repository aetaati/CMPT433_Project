/**
 * This module responsible for reading the potentiometer,
 * and setting the volume accordingly
*/


#ifndef POT_H
#define POT_H


// begins the thread that reads the potentiometer
void Potentiometer_init(void);

// ends the thread that reads the potentiometer
void Potentiometer_cleanup(void);

// Returns the A2D reading of the voltage input 'input'
// Returns value in range 0 - 4095
int Potentiometer_getA2DReading(void);


#endif