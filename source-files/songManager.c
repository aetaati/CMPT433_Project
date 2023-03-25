
/*
Author: Mehdi Esmaeilzadeh
Date: 2023-03-10
Subject: Implementation of the SongManager module
*/

#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "songManager.h"
#include "doublyLinkedList.h"
#include "audio_player.h"

static wavedata_t *pSound_currentSong = NULL;

static song_info* current_song_playing = NULL;

static void playSong(char *soundPath);
static void clean_passed_song(song_info* song);
static void iterate_through_all_songs();

static void iterate_through_all_songs() {
    struct Node * temp = doublyLinkedList_getHead();
    int i =0;
    while(temp != NULL) {
        song_info* current_tmp_song = (song_info*) temp->data;
        printf("%d), Author:%s Album:%s ",i,current_tmp_song->author_name, current_tmp_song->album);
        temp = temp->next;
        i++;
    }
}
static void playSong(char *soundPath)
{
  pSound_currentSong = malloc(sizeof(*pSound_currentSong));
  AudioPlayer_readWaveFileIntoMemory(soundPath, pSound_currentSong);
  AudioPlayer_playWAV(pSound_currentSong);
}
static void clean_passed_song(song_info* song) {
    if(song != NULL) {
        if(song->album != NULL) {
            free(song->album);
        }
        if(song->author_name != NULL) {
            free(song->author_name);
        }
        if(song->song_path != NULL) {
            free(song->song_path);
        }
        free(song);
    }
}

void songManager_init(void) {
    doublyLinkedList_init();
}

size_t songManager_currentNumberSongs(void) {
    int size = 0;
    struct Node * temp = doublyLinkedList_getHead();
    while(temp != NULL) {
        size++;
        temp = temp->next;
    } 
    return size;
}

void songManager_playSong(int song_number) {
    int n = song_number;
    struct Node * temp = doublyLinkedList_getHead();
    for(int i=0; i < n; i++) {
        if(temp == NULL) {
            break;
        }
        temp = temp->next;
    }
    if(temp == NULL) {
        printf("Song is not Existed ! \n");
    }
    else {
        current_song_playing = (song_info *)temp->data;
        playSong(current_song_playing->song_path);
    }
}

void songManager_addSongFront(song_info *song){
    doublyLinkedList_prependItem(song, sizeof(song));
    clean_passed_song(song);
}
void songManager_addSongBack( song_info * song) {
    doublyLinkedList_appendItem(song, sizeof(song));
    clean_passed_song(song);
}
// Displays all the songs
void songManager_displayAllSongs(void) {
    iterate_through_all_songs();
}

// Gets the data of the "current"
song_info *songManager_getCurrentSongPlaying(void) {
    return current_song_playing;
}

// Frees the memory for all nodes, the data, and the List struct
void songManager_cleanup(void) {
    doublyLinkedList_cleanup();
}