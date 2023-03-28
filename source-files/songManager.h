#if !defined(SONG_MANAGER_H)
#define SONG_MANAGER_H

typedef struct {
    char * song_path;
    char * author_name;
    char * album;
}song_info;


void songManager_init(void);

// TODO: REMOVE
// /* Plays the song based on the passed song #*/
// void songManager_playSong(int song_number);

// Plays the current song
void songManager_playCurrentSong(void);

/* Adds the song to the front of the list*/
void songManager_addSongStart(song_info *);
/* Adds the song to the back of the list*/
void songManager_addSongEnd(song_info *);
// Displays all the songs
void songManager_displayAllSongs(void);
/* Returns current number of the songs that are currently in the list*/
size_t songManager_getCurrentSongCount(void);

/* Returns song_info struct to the user*/
// Gets current song_playing
song_info *songManager_getCurrentSongPlaying(void);

// Move "current" to the next song
void songManager_next(void);

// Move "current" to the prev song
void songManager_prev(void);

// Sets the "current" to the song with index of "idx"
void songManager_setCurrentSong(int idx);

// Frees the memory for all data
void songManager_cleanup(void);

#endif // SONG_MANAGER_H