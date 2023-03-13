#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

void Bluetooth_init(void);

void Bluetooth_cleanup(void);

/**
 * Iterates over a list of scanned devices, displaying each 
 * devices human-readable name if available.
 * 
 * @param devices list of devices scanned by a previous call to Bluetooth_scan()
 * @param num_devices the number of devices in the list
*/
void Bluetooth_displayDevices(inquiry_info *devices, int num_devices);


/**
 * Connects to the bluetooth device with address device_address
 * 
 * @param device_address the 6 byte address of the device, in the form: XX:XX:XX:XX:XX:XX
 * @return 0 on success -1 on failure
*/
int Bluetooth_connect(bdaddr_t* device_address);


/**
 * Scans for nearby bluetooth devices
 * 
 * @param devices list to be filled with scanned devices
 * @param max_rsvp the maximum amount of devices to be scanned
 * @return the actual number of devices scanned 
 */
int Bluetooth_scan(inquiry_info* devices, int max_rsvp);


/**
 * Disconnects from the currently connected bluetooth device
*/
void Bluetooth_disconnect(void);


#endif