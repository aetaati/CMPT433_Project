#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

// max number of devices that can respond to a bluetooth scan query
#define BT_MAX_DEV_RSP 255

typedef struct{
    inquiry_info* devices;
    int num_devices;
}bluetooth_scan_t;


/**
 * Iterates over a list of scanned devices, displaying each 
 * devices human-readable name if available.
 * 
 * @param devices list of devices scanned by a previous call to Bluetooth_scan()
 * @param num_devices the number of devices in the list
*/
void Bluetooth_displayDevices(inquiry_info *devices, int num_devices);

void Bluetooth_getHumanReadableNames(bluetooth_scan_t* scanned_devices, char* names[]);


/**
 * Connects to the bluetooth device with address device_address
 * 
 * @param device_address the 6 byte address of the device, in the form: XX:XX:XX:XX:XX:XX
 * @return 0 on success -1 on failure
*/
int Bluetooth_connect(bdaddr_t* device_address);

/**
 * Pairs with a bluetooth device with device_address
 * 
 * @param device_address the address of the device in the form XX:XX:XX:XX:XX:XX
 * @return 0 on success, -1 on failure
*/
int Bluetooth_pair(bdaddr_t *device_address);


/**
 * Scans for nearby bluetooth devices
 * 
 * @param scanner a pointe to a bluetooth_scan_t strcut to hold the scanned 
 * devices. ***Must Free scanner->devices before scanner***
 * 
 * @return -1 if there is an error. 
 */
int Bluetooth_scan(bluetooth_scan_t* scanner);


/**
 * Disconnects from the currently connected bluetooth device
*/
void Bluetooth_disconnect(void);


#endif