#include "bluetooth.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>


// Bluetooth Module Threading
void* bluetoothThread(void* arg);
static bool stopping = false;
static pthread_t bluetooth_thread_id;
pthread_mutex_t mtx_bluetooth = PTHREAD_MUTEX_INITIALIZER;


void Bluetooth_init(void){

    pthread_create(&bluetooth_thread_id, NULL, bluetoothThread, NULL);

}



void* bluetoothThread(void* args){
    while(!stopping){

        printf("hello\n");




    }

    return NULL;
}