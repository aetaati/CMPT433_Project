/*
Author: Mehdi Esmaeilzadeh
Date: 2023-03-12
Subject: Definition of the MenuManager module
*/

#if !defined(_MENUMANAGER_H)
#define _MENUMANAGER_H

#define VOLUME_CHANGE_SIZE 5
#define TEMPO_CHANGE_SIZE 5

#define MAX_VOLUME 100
#define MIN_VOLUME 0

#define MAX_TEMPO 300
#define MIN_TEMPO 40

#define INPUT_CHECK_WAIT_TIME 5
#define DEBOUNCE_WAIT_TIME 100

#include <stdbool.h>

enum eCurrentSong
{
   NO_SONG = 0,
   SONG_NUM_ONE,
   SONG_NUM_TWO,
   SONG_NUM_THREE,
   MAX_NUM_SONGS
};

// #define CYN_HARD_FILE "wave-files/100056__menegass__gui-drum-cyn-hard.wav"
// #define TOM_LO_FILE "wave-files/100065__menegass__gui-drum-tom-lo-soft.wav"
// #define CO_FILE "wave-files/100055__menegass__gui-drum-co.wav"


// Initilaize all the modules used in the menu
void MenuManager_init(void);

// Starts playing a song based on the passed "mode"
void MenuManager_StartSong(enum eCurrentSong mode);

// Stops the playing the song
void MenuManager_StopSong(void);

// Clean Up all the modules used in the menu
void MenuManager_cleanup(void);

/********************FUNCTION THAT WE MIGHT USE **************************/

// Update the tempo by "changeSize"
// Increases tempo if "isIncrease" is true, decreases otherwise
// Note: "changeSize" should be positive or else this function would do nothing
// Note: this function is thread safe to handle simultaneous tempo change
//    from the web interface and the joyStick
//
// void MenuManager_UpdateTempo(int changeSize, bool isIncrease);

/**********************************************/

// Change the volume by "changeSize"
// Increases volume if "isIncrease" is true, decreases otherwise
// Note: "changeSize" should be positive or else this function would do nothing
// Note: this function is thread safe to handle simultaneous volume change
//    from the web interface and the joyStick
void MenuManager_UpdateVolume(int changeSize, bool isIncrease);


// Get the current tempo
// Note: this function is thread safe
// int MenuManager_GetCurrentTempo(void);

// Get the current volume
// Note: this function is thread safe
int MenuManager_GetCurrentVolume(void);

// Returns the current song playing by the user
int MenuManager_GetCurrentSongPlaying();

#endif // _MENUMANAGER_H
