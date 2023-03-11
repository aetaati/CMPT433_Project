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
#define SCAN_LENGTH 8

// Bluetooth Module Threading
void *bluetoothThread(void *arg);
static bool stopping = false;
static pthread_t bluetooth_thread_id;
pthread_mutex_t mtx_bluetooth = PTHREAD_MUTEX_INITIALIZER;

// Bluetooth Info
int bt_adapter_id;
int bt_adapter_fd;
wavedata_t song;

/*typedef struct{
    int num_devices;
    inquiry_info* devices;
}scanned_devices;*/

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
    for (int i = 0; i < num_devices; i++){
        // baddr is the 6 byte address of the bt device. format: XX:XX:XX:XX:XX:XX
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));

        // send request to device with address = baddr for human readable name
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
    char input[15] = {0};
    while (!stopping)
    {  
        printf("Scanning for bluetooth devices...\n");
        inquiry_info *scanned_devices = malloc(MAX_DEV_RSP * sizeof(inquiry_info));
        if (scanned_devices == NULL)
        {
            fprintf(stderr, "failed to allocate memory for scanned devices\n");
            exit(1);
        }
        // scan for bt devices
        int num_rsp = hci_inquiry(bt_adapter_id, SCAN_LENGTH, MAX_DEV_RSP, NULL, &scanned_devices, IREQ_CACHE_FLUSH);
        if (num_rsp < 0)
        {
            fprintf(stderr, "hci_inquiry: error scanning devices");
        }

        Bluetooth_displayAvailableDevices(scanned_devices, num_rsp);

        printf("Choose a device to connect to\n");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            fprintf(stderr, "error reading from stdin");
        }
        sscanf(input, "%d", &selection);

        printf("connecting to device...\n");
        if (Bluetooth_connect(&(scanned_devices + selection)->bdaddr) != 0){
            printf("error connecting to device\n");
        }
        printf("Connected!\n");


        sleep(5);

        printf("disconnecting...\n");
        Bluetooth_disconnect();
        printf("disconnected!\n");
        


        //AudioMixer_init("default");
        //AudioMixer_readWaveFileIntoMemory("som-liveletlive.wav", &song);
        //AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100060__menegass__gui-drum-splash-hard.wav", &song);
        //AudioMixer_queueSound(&song);

        memset(input, 0, sizeof(input));
        free(scanned_devices);
        scanned_devices = NULL;
    }

    return NULL;
}

void Bluetooth_disconnect(void){
    runCommand("bluetoothctl disconnect");

}

int Bluetooth_connect(bdaddr_t* device_address){
    
    char addr[19] = { 0 };
    ba2str(device_address, addr);
    char* tmp  = "bluetoothctl connect ";
    char* bt_command = calloc(sizeof(char), strlen(tmp) + 19 );
   
    strcat(strcat(bt_command, tmp), addr);
        
      
    return runCommand(bt_command);
}




int runCommand(char* command){

    FILE* pipe = popen(command, "r");

    char buffer[1024];
    while(!feof(pipe) && !ferror(pipe)){
        if(fgets(buffer, sizeof(buffer), pipe)){
            break;
        }
    }

    int exit_code = WEXITSTATUS(pclose(pipe));
    if(exit_code != 0){
        fprintf(stderr, "Unable to execute command:\n  command: %s\n  exit code: %d\n", command, exit_code);
        return(1);
    }

    return(0);
}


void print_record(sdp_record_t *record) {
    printf("Handle: %d\n", record->handle);

    printf("Search Pattern:\n");
    sdp_list_t *pattern = record->pattern;
    while (pattern) {
        uuid_t uuid;
        sdp_uuid128_create(&uuid, pattern->data);
        char uuid_str[128];
        sdp_uuid2strn(&uuid, uuid_str, 128);
        printf("  %s\n", uuid_str);
        pattern = pattern->next;
    }

    /*printf("Attribute List:\n");
    sdp_list_t *attrlist = record->attrlist;
    while (attrlist) {
        uint16_t attr_id = attrlist->id;
        sdp_data_t *attr_data = attrlist->data;
        printf("  Attribute ID: 0x%04x\n", attr_id);
        printf("  Attribute Data:\n");
        while (attr_data) {
            switch (attr_data->type) {
                case SDP_UUID128:
                case SDP_UUID32:
                case SDP_UUID16:
                {
                    uuid_t uuid;
                    sdp_uuid128_create(&uuid, attr_data->val.uuid128);
                    char uuid_str[128];
                    sdp_uuid2strn(&uuid, uuid_str, 128);
                    printf("    UUID: %s\n", uuid_str);
                    break;
                }
                case SDP_UINT8:
                    printf("    UINT8: %d\n", attr_data->val.uint8);
                    break;
                case SDP_UINT16:
                    printf("    UINT16: %d\n", attr_data->val.uint16);
                    break;
                case SDP_UINT32:
                    printf("    UINT32: %d\n", attr_data->val.uint32);
                    break;
                case SDP_INT8:
                    printf("    INT8: %d\n", attr_data->val.int8);
                    break;
                case SDP_INT16:
                    printf("    INT16: %d\n", attr_data->val.int16);
                    break;
                case SDP_INT32:
                    printf("    INT32: %d\n", attr_data->val.int32);
                    break;
                case SDP_STRING:
                    printf("    STRING: %s\n", attr_data->val.str);
                    break;
                default:
                    printf("    Unknown attribute data type: 0x%x\n", attr_data->type);
                    break;
            }
            attr_data = attr_data->next;
        }
        attrlist = attrlist->next;
    }*/


    char svclass_str[128];
    sdp_uuid2strn(&record->svclass, svclass_str, 128);
    printf("Service Class: %s\n", svclass_str);
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