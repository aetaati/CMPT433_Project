#if !defined(_MENUMANAGER_H)
#define _MENUMANAGER_H


#include <stdbool.h>
#include "songManager.h"

/**
 * Enum to keep track of current Menu to display
*/
typedef enum
{
  MAIN_MENU,
  SONGS_MENU,
  BLUETOOTH_MENU,
  BTSCAN_MENU,
  SETTINGS_MENU,
  SONGINFO_MENU,
  NUM_MENUS
} MENU;

/**
 * Enum to keep track Main Menu Options
*/
typedef enum
{
  SONGS_OPT,
  BLUETOOTH_OPT,
  SETTINGS_OPT,
  POWEROFF_OPT,
  NUM_MAIN_OPTIONS
} MAIN_OPTIONS;


/**
 * Enum to keep track of Bluetooth menu options
*/
typedef enum{
  BTMENU_CONNECT,
  BTMENU_DISCONNECT,
  NUM_BT_OPTIONS
} BLUETOOTH_OPTIONS;


// Initilaize all the modules used in the menu
void MenuManager_init(void);

// Stops the playing the song
void MenuManager_StopSong(void);

// Clean Up all the modules used in the menu
void MenuManager_cleanup(void);

// Returns the current song playing by the user
song_info* MenuManager_GetCurrentSongPlaying();

#endif // _MENUMANAGER_H
