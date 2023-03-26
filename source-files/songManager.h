#if !defined(SONG_MANAGER_H)
#define SONG_MANAGER_H

typedef struct {
    char * song_path;
    char * author_name;
    char * album;
}song_info;


void songManager_init(void);
/* Plays the song based on the passed song #*/
void songManager_playSong(int song_number);
/* Adds the song to the front of the list*/
void songManager_addSongFront(song_info *);
/* Adds the song to the back of the list*/
void songManager_addSongBack (song_info *);
// Displays four the songs
void songManager_displaySongs(int current_song, int from_song_number);
/* Returns current number of the songs that are currently in the list*/
size_t songManager_currentNumberSongs(void);

/* Returns song_info struct to the user*/
// Gets current song_playing
song_info *songManager_getCurrentSongPlaying(void);

// Frees the memory for all data
void songManager_cleanup(void);

#endif // SONG_MANAGER_H