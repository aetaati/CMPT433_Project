#include "bluetooth.h"
#include "audio-mixer.h"

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
#define MAX_DEV_RSP 255

// the duration of the bluetooth scan = 1.28s * SCAN_LENGTH
#define SCAN_LENGTH 4

// Bluetooth Module Threading
void *bluetoothThread(void *arg);
static bool stopping = false;
static pthread_t bluetooth_thread_id;
pthread_mutex_t mtx_bluetooth = PTHREAD_MUTEX_INITIALIZER;

// Bluetooth Info
int bt_adapter_id;
int bt_adapter_fd;
wavedata_t song;

void Bluetooth_init(void)
{
    // get default bt adapter
    bt_adapter_id = hci_get_route(NULL);
    checkError();
    bt_adapter_fd = hci_open_dev(bt_adapter_id);
    checkError();

    pthread_create(&bluetooth_thread_id, NULL, bluetoothThread, NULL);
}

void Bluetooth_displayAvailableDevices(inquiry_info *devices, int num_devices)
{
    char addr[19] = {0};
    char name[256] = {0};
    for (int i = 0; i < num_devices; i++)
    {
        // baddr is the 6 byte address of the bt device. format: XX:XX:XX:XX:XX:XX
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));

        // send request to device with address equal to baddr, for human readable name
        if (hci_read_remote_name(bt_adapter_fd, &(devices + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        printf("[%d]: %s\n", i, name);
    }
}

void *bluetoothThread(void *args)
{
    int selection;
    char* device_names[MAX_DEV_RSP];
    char input[15] = {0};
    while (!stopping)
    {

        int num_scanned = Bluetooth_scan(device_names);
        for(int i = 0; i < num_scanned; i++){
            printf("device: %s\n", *(device_names+i));
        }

        printf("Choose a device to connect to\n");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            fprintf(stderr, "error reading from stdin");
        }
        sscanf(input, "%d", &selection);

        /*printf("connecting to device...\n");
        if (Bluetooth_connect(&(scanned_devices + selection)->bdaddr) != 0)
        {
            printf("error connecting to device\n");
        }
        printf("Connected!\n");*/

        sleep(5);

        printf("disconnecting...\n");
        Bluetooth_disconnect();
        printf("disconnected!\n");

        // AudioMixer_init("default");
        // AudioMixer_readWaveFileIntoMemory("som-liveletlive.wav", &song);
        // AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100060__menegass__gui-drum-splash-hard.wav", &song);
        // AudioMixer_queueSound(&song);

        memset(input, 0, sizeof(input));
        
        
    }

    return NULL;
}

// returns the amount of devices that responded to a scan
// must free each string in returned list
int Bluetooth_scan(char **devices)
{
    printf("Scanning for bluetooth devices...\n");
    inquiry_info *scanned_devices = malloc(MAX_DEV_RSP * sizeof(inquiry_info));
    if (scanned_devices == NULL)
    {
        fprintf(stderr, "failed to allocate memory for scanned devices\n");
        return(-1);
    }

    // scan for bt devices
    int num_rsp = hci_inquiry(bt_adapter_id, SCAN_LENGTH, MAX_DEV_RSP, NULL, &scanned_devices, IREQ_CACHE_FLUSH);
    if (num_rsp < 0)
    {
        fprintf(stderr, "hci_inquiry: error scanning devices");
    }

    char name[256] = {0};
    for (int i = 0; i < num_rsp; i++)
    {
        memset(name, 0, sizeof(name));

        // send request to device with address equal to baddr, for human readable name
        if (hci_read_remote_name(bt_adapter_fd, &(scanned_devices + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        *(devices+i) = calloc(sizeof(char) , strlen(name) + 1);
        strcpy(*(devices+i), name);
    }

    free(scanned_devices);
    scanned_devices = NULL;
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
        fprintf(stderr, "Unable to execute command:\n  command: %s\n  exit code: %d\n", command, exit_code);
        return (1);
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