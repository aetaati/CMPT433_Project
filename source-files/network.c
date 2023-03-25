//
// Created by: Amirhossein Etaati
// Date: 2023-02-07
// Purpose: Declarations of the network module to send and respond to the user commands across the network using UPD packets
// Note: Some parts of this code is adapted from: UDP Listening program on port 22110 * By Brian Fraser, Modified from Linux Programming Unleashed (book)
//

#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h> // for strncmp()
#include <unistd.h> // for close()
#include <ctype.h>

#define MSG_MAX_LEN 1024
#define MSG_ACK "ACK"
#define MSG_ACK_LEN 4
#define PORT 12345
#define MAX_SAMPLES_LINE 20

// #define MAX_HISTORY_STR_SIZE 32768
#define MAX_LINE_STR_SIZE 256

static pthread_t thread_id;

// to handle commands
enum eWebCommands
{
    COMMAND_VOLUME_UP = 0,
    COMMAND_VOLUME_DOWN,
    COMMAND_SONG_NEXT,
    COMMAND_SONG_PREVIOUS,
    COMMAND_STOP,
    UNKNOWN_COMMAND,
    COMMAND_TOTAL_COUNT // Total number of available commands ??
};

// parse the received command
static enum eWebCommands parse_command(char messageRx[MSG_MAX_LEN])
{
    if (strncmp(messageRx, "volume_up", strlen("volume_up")) == 0)
    {
        return COMMAND_VOLUME_UP;
    }
    else if (strncmp(messageRx, "volume_down", strlen("volume_down")) == 0)
    {
        return COMMAND_VOLUME_DOWN;
    }
    else if (strncmp(messageRx, "song_next", strlen("song_next")) == 0)
    {
        return COMMAND_SONG_NEXT;
    }
    else if (strncmp(messageRx, "song_previous", strlen("song_previous")) == 0)
    {
        return COMMAND_SONG_PREVIOUS;
    }
    else if (strncmp(messageRx, "stop", strlen("stop")) == 0)
    {
        return COMMAND_STOP;
    }
    else
    {
        return UNKNOWN_COMMAND;
    }
}

// run the commands and return the appropriate string to be sent back
// returns NULL rerun is required
// the caller should call free()
static char *run_command(enum eWebCommands cur_command)
{
    char *result = NULL;
    result = malloc(MSG_ACK_LEN * sizeof(char));
    snprintf(result, MSG_ACK_LEN, MSG_ACK);
    if (cur_command == COMMAND_VOLUME_UP)
    {
        // TODO call the volume module to increase the volume
        return result;
    }
    else if (cur_command == COMMAND_VOLUME_DOWN)
    {
        // TODO call the volume module to decrease the volume
        return result;
    }
    else if (cur_command == COMMAND_SONG_NEXT)
    {
        // TODO call the song module to go to the next song
        return result;
    }
    else if (cur_command == COMMAND_SONG_PREVIOUS)
    {
        // TODO call the song module to the previous song
        return result;
    }
    else if (cur_command == COMMAND_SONG_PREVIOUS)
    {
        // TODO stop all the other threads and modules
        return result;
    }
    else if (cur_command == COMMAND_STOP)
    {
        // TODO stop all the other threads and modules
        return result;
    }
    else
    {
        return NULL; // TODO: ??
    }
}

// thread function to manage networking logics
static void *network_thread(void *params)
{

    // UPD setups (server/receiver side)

    // initialize Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                // connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
    sin.sin_port = htons(PORT);              // host to network short

    // create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // bind the specified port (PORT)
    bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));

    // check for errors
    if (socketDescriptor == -1)
    {
        printf("ERROR: Failed to create the socket\n");
        exit(-1);
    }

    int n = -1;

    bool local_network_cond = true;
    while (local_network_cond)
    {

        // create variables required for receiving
        struct sockaddr_in sinRemote; // set to the sender/clients's address. Use to reply
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];

        // listen for the commands (blocking)

        // buffer size: maximum length minus one to allow null termination (string data)
        int buffer_size = MSG_MAX_LEN - 1;
        int bytesRx = recvfrom(socketDescriptor, messageRx, buffer_size, 0, (struct sockaddr *)&sinRemote, &sin_len);
        // Check for errors
        if (bytesRx == -1)
        {
            printf("ERROR: Failed to receive. Data is not available and socket is in nonblocking mode\n");
            exit(-1);
        }

        // handle the received message ///////////////////////////////////////////////////////////////////////

        // make the received message null terminated so string functions work
        messageRx[bytesRx] = 0;

        // if command is not 'enter' (!= "\n")
        if (bytesRx > 1)
        {
            // get rid of \n in the command
            for (size_t i = 0; i < strlen(messageRx); i++)
            {
                if (messageRx[i] == '\n')
                {
                    messageRx[i] = '\0';
                }
            }
        }

        // extract the command from the message received
        enum eWebCommands curr_command = parse_command(messageRx);

        // compose the response message

        char *response = run_command(curr_command);

        // TODO: for now all the replies are ACK; Kept for potential later use
        if (strlen(response) <= MSG_MAX_LEN)
        {
            // send in one packet
            char messageTx[MSG_MAX_LEN];
            snprintf(messageTx, MSG_MAX_LEN, "%s", response);

            // transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        }
        else
        {
            // read response line by line
            // delimeter
            const char s[2] = "\n";
            char *messageTx_token; // each token is a line
            // get the first token/line
            messageTx_token = strtok(response, s);

            // walk through other tokens /line
            while (messageTx_token != NULL)
            {
                // adding the lost new line character
                messageTx_token[strlen(messageTx_token) - 1] = '\n';

                // transmit a reply:
                sin_len = sizeof(sinRemote);
                sendto(socketDescriptor, messageTx_token, strlen(messageTx_token), 0, (struct sockaddr *)&sinRemote, sin_len);

                messageTx_token = strtok(NULL, s);
            }
        }
        free(response);

        // check termination condition
        if (curr_command == COMMAND_STOP)
        {
            local_network_cond = false;
        }
    }

    // close the socket
    close(socketDescriptor);

    return NULL;
}

// start a new thread to listen to the incoming packets and responds to the commands
void Network_init()
{
    // start network thread
    pthread_create(&thread_id, NULL, &network_thread, NULL);
}

// stop network thread
void Network_cleanup()
{
    // wait for the thread to finish
    pthread_join(thread_id, NULL);
}
