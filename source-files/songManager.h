#if !defined(SONG_MANAGER_H)
#define SONG_MANAGER_H

typedef struct {
    char * song_path;
    char * author_name;
    char * album;
}song_info;

// Returns a pointer to the newly allocated empty list
// Note: caller should call doublyLinkedList_cleanup() to free the memory
void songManager_init(void);

void songManager_playSong(int song_number);

void songManager_addSongFront(song_info *);
void songManager_addSongBack( song_info *);
// Displays all the songs
void songManager_displayAllSongs(void);

// Gets current song_playing
song_info *songManager_getCurrentSongPlaying(void);

// Frees the memory for all nodes, the data, and the List struct
void songManager_cleanup(void);

#endif // SONG_MANAGER_H