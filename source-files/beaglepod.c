#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


#include "sleep.h"
#include "shutdown.h"
#include "bluetooth.h"
#include "audio_player.h"
#include "gpio.h"
#include "joystick.h"
#include "menuManager.h"
#include "volume.h"
#include "lcd_4line.h"
#include "network.h"


#define SONG "songs/som-liveletlive.wav"
#define SONG3 "songs/hunnybee.wav"
#define SONG4 "songs/moves.wav"
#define SONG5 "songs/kiss-from-rose.wav"


int main(int argc, char const *argv[])
{

    AudioPlayer_init();
    Potentiometer_init();
    MenuManager_init();
    Network_init();
    songManager_init();
    
    
    Shutdown_init();
    Shutdown_waitForShutdown();

    songManager_cleanup();
    Network_cleanup();
    MenuManager_cleanup();
    Potentiometer_cleanup();
    AudioPlayer_cleanup();

    return 0;
}
