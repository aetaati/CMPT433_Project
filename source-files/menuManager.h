/*
Author: Mehdi Esmaeilzadeh
Date: 2023-02-25
Subject: Definition of the MenuManager module to get the current accelerometer action
*/

#if !defined(_MENUMANAGER_H)
#define _MENUMANAGER_H

#define VOLUME_CHANGE_SIZE 5
#define TEMPO_CHANGE_SIZE 5

#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define MAX_TEMPO 300
#define MIN_TEMPO 40

#include <stdbool.h>

enum eCurrentSong
{
   NO_SONG = 0,
   SONG_NUM_ONE,
   SONG_NUM_TWO,
   SONG_NUM_THREE,
   MAX_NUM_SONGS
};

// enum eCurrentRockSound
// {
//    HI_HAT_BASE = 0,
//    HI_HAT_ONE,
//    HI_HAT_SNARE,
//    HI_HAT_TWO,
//    MAX_NUM_ROCKS
// };

// enum eCurrentMadeUpSound
// {
//    CYN_HARD,
//    TOM_LO,
//    CO,
//    MAX_NUM_MADE_UP
// };

// #define BASE_DRUM_FILE "wave-files/100051__menegass__gui-drum-bd-hard.wav"
// #define HI_HAT_FILE "wave-files/100053__menegass__gui-drum-cc.wav"
// #define SNARE_FILE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

// #define CYN_HARD_FILE "wave-files/100056__menegass__gui-drum-cyn-hard.wav"
// #define TOM_LO_FILE "wave-files/100065__menegass__gui-drum-tom-lo-soft.wav"
// #define CO_FILE "wave-files/100055__menegass__gui-drum-co.wav"

void MenuManager_init(void);

// Handles Joystick and Accelerometer
void MenuManager_HandleInputs(void);

// Starts a thread to start playing the "mode" song
void MenuManager_StartSong(enum eCurrentSong mode);

// Stops the thread that is playing song
void MenuManager_StopSong(void);

// // Plays a
// void MenuManager_PlaySingleSound(char *soundPath);

void MenuManager_cleanup(void);

// Update the tempo by "changeSize"
// Increases tempo if "isIncrease" is true, decreases otherwise
// Note: "changeSize" should be positive or else this function would do nothing
// Note: this function is thread safe to handle simultaneous tempo change
//    from the web interface and the joyStick
// void MenuManager_UpdateTempo(int changeSize, bool isIncrease);

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

int MenuManager_GetCurrentSongPlaying();

#endif // _MENUMANAGER_H
