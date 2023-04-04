/*
Purpose: Declaration of the Sleep module to handle required waitings
*/

#if !defined(_SLEEP_H)
#define _SLEEP_H

// Puts thread to sleep for 'delayInMs' milliseconds
void Sleep_ms(long long delayInMs);

// Puts thread to sleep for 's' seconds and  'ns' nanoseconds
// nanosleep wrapper function - accepts seconds and nanoseconds to construct delay
// Note: Adapted from the 16x2CharacterLCDThroughGPIO student guide
    // URL: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/16x2CharacterLCDThroughGPIO.pdf
void Sleep_ns(int s, int ns);

#endif // _SLEEP_H
