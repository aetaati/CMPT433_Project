#include "bluetooth.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <errno.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

// max number of devices that can respond to a bluetooth scan query
#define MAX_DEV_RSP 255 

// the duration of the bluetooth scan = 1.28s * SCAN_LENGTH
#define SCAN_LENGTH 8


// Bluetooth Module Threading
void* bluetoothThread(void* arg);
static bool stopping = false;
static pthread_t bluetooth_thread_id;
pthread_mutex_t mtx_bluetooth = PTHREAD_MUTEX_INITIALIZER;

// Bluetooth Info
int bt_adapter_id;
int bt_sock_fd;

/*typedef struct{
    int num_devices;
    inquiry_info* devices;
}scanned_devices;*/





void Bluetooth_init(void){
    // get default bt adapter
    bt_adapter_id = hci_get_route(NULL);
    checkError();
    bt_sock_fd = hci_open_dev(bt_adapter_id);
    checkError();
    pthread_create(&bluetooth_thread_id, NULL, bluetoothThread, NULL);
}


void Bluetooth_displayAvailableDevices(inquiry_info* devices, int num_devices){
    char addr[19] = { 0 };
    char name[256] = { 0 };
    for (int i = 0; i < num_devices; i++) {
        ba2str(&(devices+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(bt_sock_fd, &(devices+i)->bdaddr, sizeof(name), name, 0) < 0){
            strcpy(name, "[unknown]");
        }
        printf("[%d]: %s  %s\n", i, addr, name);
    }

}




void* bluetoothThread(void* args){
    int selection;
    char input[15] = { 0 };
    while(!stopping){
        printf("Scanning for bluetooth devices...\n");
        inquiry_info* scanned_devices = malloc(MAX_DEV_RSP * sizeof(inquiry_info));
        if(scanned_devices == NULL){
            fprintf(stderr, "failed to allocate memory for scanned devices\n");
            exit(1);
        }

        int num_rsp = hci_inquiry(bt_adapter_id, SCAN_LENGTH, MAX_DEV_RSP, NULL, &scanned_devices, IREQ_CACHE_FLUSH);
        Bluetooth_displayAvailableDevices(scanned_devices, num_rsp);

        
        printf("Choose a device to connect to\n");
        if(fgets(input, sizeof(input), stdin) == NULL){
            fprintf(stderr, "error reading from stdin");
        }

        sscanf(input, "%d", &selection);
        printf("choice %d\n", selection);
        memset(input, 0 , sizeof(input));
        free(scanned_devices);
        scanned_devices = NULL;


    }

    return NULL;
}




void checkError(void){
    switch (errno) {
        case EADDRNOTAVAIL:
            fprintf(stderr, "Error: Bluetooth hardware address not available\n");
            exit(1);
            break;
        case EFAULT:
            fprintf(stderr, "Error: Invalid memory location for Bluetooth hardware address\n");
            exit(1);
            break;
        case EINVAL:
            fprintf(stderr, "Error: Invalid Bluetooth device identifier\n");
            exit(1);
            break;
        case EBADF:
            fprintf(stderr, "Error: Invalid Bluetooth device id\n");
            exit(1);
            break;
        case EBUSY:
            fprintf(stderr, "Error: Bluetooth adapter already in use\n");
            exit(1);
            break;
        case EIO:
            fprintf(stderr, "Error: IO error when attempting to open Bluetooth adapter\n");
            exit(1);
            break;
        case ENODEV:
            fprintf(stderr, "Error: Bluetooth adapter does not exist or is unavailable\n");
            exit(1);
            break;
        case ENOMEM:
            fprintf(stderr, "Error: Insufficient memory to allocate Bluetooth resources\n");
            exit(1);
            break;
        case ENOTTY:
            fprintf(stderr, "Error: Specified fd is not a TTY device\n");
            exit(1);
            break;
        default:
            // no error
            break;
    }
    
}