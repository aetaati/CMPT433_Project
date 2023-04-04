#if !defined(SONG_MANAGER_H)
#define SONG_MANAGER_H
#include "audio_player.h"

typedef struct {
    char * song_path;
    char * author_name;
    char * album;
    char * song_name;
    wavedata_t * pSong_DWave; 
}song_info;

typedef enum
{
  CURSOR_LINE_ONE,
  CURSOR_LINE_TWO,
  CURSOR_LINE_THREE,
  CURSOR_LINE_FOUR,
  CURSOR_LINE_NOT_SET,
  NUM_CURSOR_POSITIONS
} SONG_CURSOR_LINE;


void songManager_init(void);
/* Plays the song that the cursor is pointing at */
void songManager_playSong();
/* Adds the song to the front of the list*/
void songManager_addSongFront(song_info *);
/* Adds the song to the back of the list*/
void songManager_addSongBack (song_info *);
/*Resets the songManager when user exits the song menu */
void songManager_reset(void);
/* Moves the cursor up */
void songManager_moveCursorDown(void);
/* Moves the cursor down */
void songManager_moveCursorUp(void);
/* Displays the songs */
void songManager_displaySongs();

/* Song Mananger Delete a song*/
void songManager_deleteSong();

/* Returns song_info struct to the user*/
// Gets current song_playing
song_info *songManager_getCurrentSongPlaying(void);

// Frees the memory for all data
void songManager_cleanup(void);

#endif // SONG_MANAGER_H