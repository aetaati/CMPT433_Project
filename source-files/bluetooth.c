#include "bluetooth.h"
#include "audio_player.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>



// max number of devices that can respond to a bluetooth scan query


// the duration of the bluetooth scan = 1.28s * SCAN_LENGTH
#define SCAN_LENGTH 4

// Bluetooth Module Threading
//void *bluetoothThread(void *arg);
//static bool stopping = false;
//static pthread_t bluetooth_thread_id;
pthread_mutex_t mtx_bluetooth = PTHREAD_MUTEX_INITIALIZER;

// Bluetooth Info
int bt_adapter_id;
int bt_adapter_fd;

// Private functions
void checkError(void);
int runCommand(char* command);



void Bluetooth_init(void)
{
    // get default bt adapter
    bt_adapter_id = hci_get_route(NULL);
    checkError();
    bt_adapter_fd = hci_open_dev(bt_adapter_id);
    checkError();

    //pthread_create(&bluetooth_thread_id, NULL, bluetoothThread, NULL);
}


/*void *bluetoothThread(void *args)
{
    int selection;
    inquiry_info* devices;
    char input[15] = {0};
    while (!stopping)
    {
        devices = malloc(MAX_DEV_RSP * sizeof(inquiry_info));
        int num_scanned = Bluetooth_scan(devices, MAX_DEV_RSP);
        Bluetooth_displayDevices(devices, num_scanned);

        printf("Choose a device to connect to\n> ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            fprintf(stderr, "error reading from stdin");
        }
        sscanf(input, "%d", &selection);



        printf("connecting to device...\n");
        if (Bluetooth_connect(&(devices + selection)->bdaddr) != 0)
        {
            printf("error connecting to device\n");
        }
        else{
            printf("Connected!\n");
        }
        
        wavedata_t song;
        AudioPlayer_readWaveFileIntoMemory(SONG , &song);
        AudioPlayer_playWAV(&song);
        sleep(30);
        printf("hey\n");
        AudioMixer_setVolume(0.9);
        sleep(100);
        AudioMixer_setVolume(0.9);
        sleep(10);
        printf("disconnecting...\n");
        Bluetooth_disconnect();
        printf("disconnected!\n");

        //memset(input, 0, sizeof(input));
    }
    return NULL;
}
*/

void Bluetooth_displayDevices(inquiry_info *devices, int num_devices)
{
    char addr[19] = {0};
    char name[256] = {0};
    for (int i = 0; i < num_devices; i++)
    {
        // bdaddr is the 6 byte address of the bluetooth device. format: XX:XX:XX:XX:XX:XX
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));

        // send request to device with address equal to bdaddr, for human readable name
        if (hci_read_remote_name(bt_adapter_fd, &(devices + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        printf("[%d]: %s\n", i, name);
    }
}


int Bluetooth_pair(bdaddr_t *device_address){
    char addr[19] = {0};
    ba2str(device_address, addr);
    char *tmp = "bluetoothctl pair ";
    char *bt_command = calloc(sizeof(char), strlen(tmp) + 19);

    strcat(strcat(bt_command, tmp), addr);

    int result = runCommand(bt_command);
    free(bt_command);
    bt_command = NULL;

    return result;
}


int Bluetooth_scan(inquiry_info *scanned_devices, int n)
{
    printf("Scanning for bluetooth devices...\n");
    if (scanned_devices == NULL)
    {
        fprintf(stderr, "failed to allocate memory for scanned devices\n");
        return(-1);
    }

    // scan for bluetooth devices
    int num_rsp = hci_inquiry(bt_adapter_id, SCAN_LENGTH, n, NULL, &scanned_devices, IREQ_CACHE_FLUSH);
    if (num_rsp < 0)
    {
        fprintf(stderr, "hci_inquiry: error scanning devices");
    }
    return(num_rsp);

}


void Bluetooth_disconnect(void)
{
    runCommand("bluetoothctl disconnect");
}


int Bluetooth_connect(bdaddr_t *device_address)
{

    char addr[19] = {0};
    ba2str(device_address, addr);
    char *tmp = "bluetoothctl connect ";
    char *bt_command = calloc(sizeof(char), strlen(tmp) + 19);

    strcat(strcat(bt_command, tmp), addr);

    int result = runCommand(bt_command);
    free(bt_command);
    bt_command = NULL;

    return result;
}



// Private Function Definitions
int runCommand(char *command)
{
    FILE *pipe = popen(command, "r");

    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe))
        {
            break;
        }
    }

    int exit_code = WEXITSTATUS(pclose(pipe));
    if (exit_code != 0)
    {
        return (-1);
    }

    return (0);
}

void checkError(void)
{
    switch (errno)
    {
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