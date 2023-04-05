#if !defined(_NETWORK_H_)
#define _NETWORK_H_

// Start a new thread to listen to the incoming packets and responds to the commands
void Network_init();

// Wait until network thread stops
void Network_cleanup();


#endif // _NETWORK_H_
