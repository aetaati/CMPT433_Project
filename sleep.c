/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Definition of the Sleep module to handle required waitings
Author: Amirhossein Etaati
Date: 2023-03-04
*/

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "sleep.h"

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


void Sleep_ns(int s, int ns)
{
    struct timespec delay = {s, ns};
    nanosleep(&delay, (struct timespec*) NULL);
}