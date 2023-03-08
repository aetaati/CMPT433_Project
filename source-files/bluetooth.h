#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void Bluetooth_init(void);

void Bluetooth_cleanup(void);

void Bluetooth_displayAvailableDevices(inquiry_info* devices, int num_devices);

void checkError(void);

#endif