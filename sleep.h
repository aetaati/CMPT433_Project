#if !defined(_SLEEP_H)
#define _SLEEP_H

// Puts thread to sleep for 'delayInMs' milliseconds
void Sleep_ms(long long delayInMs);

// Puts thread to sleep for 's' seconds and  'ns' nanoseconds
void Sleep_ns(int s, int ns);


#endif // _SLEEP_H
