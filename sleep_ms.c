#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "sleep_ms.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


void Sleep_ms(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}