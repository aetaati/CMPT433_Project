#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

void Bluetooth_init(void);

void Bluetooth_cleanup(void);

void Bluetooth_displayAvailableDevices(inquiry_info* devices, int num_devices);

int Bluetooth_connect(bdaddr_t* device_address);

int Bluetooth_scan(char **devices);

void Bluetooth_disconnect(void);

void checkError(void);

void print_record(sdp_record_t *record);

int runCommand(char* command);

#endif