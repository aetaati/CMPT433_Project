/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Definitions of a general doublyLinkedList
Author: Amirhossein Etaati
Use cases: 1) Store the name of the songs
Date: 2023-03-16
*/

#if !defined(HASH_MAP_H)
#define HASH_MAP_H
#include <stdbool.h>

typedef enum
{
  SONG_LIST,
  ALBUM_LIST,
  ARTIST_LIST,
  NUM_KEYS_LIST
} HASH_MAP_KEY;


// Returns a pointer to the newly allocated empty list
// Note: caller should call doublyLinkedList_cleanup() to free the memory
void hashMap_init(void);

// Returns true if the list is empty
bool hashMap_isEmpty(HASH_MAP_KEY key);

// struct Node* doublyLinkedList_getHead(void);

// Adds the item "src" with size of "size" to the head of the list
void hashMap_prependItem(HASH_MAP_KEY key,void *src, unsigned int size);

// Adds the item "src" with size of "size" to the tail of the list
void hashMap_appendItem(HASH_MAP_KEY key,void *src, unsigned int size);

// Updates the "current" field of the list to the next node
// Returns true if update is successful, false if the list is empty
bool hashMap_next(HASH_MAP_KEY key);

// Updates the "current" field of the list to the previous node
// Returns true if update is successful, false if the list is empty
bool hashMap_prev(HASH_MAP_KEY key);

// Returns the element at index "idx" or NULL if idx is out of bounds
// Note: the list is zero-indexed
void *hashMap_getElementAtIndex(HASH_MAP_KEY key,int idx);

// Returns the data of the "current"
void *hashMap_getCurrentElement(HASH_MAP_KEY key);

// Frees the memory for all nodes, the data, and the List struct
void hashMap_cleanup(void);

// Resets the current pointer to point to the element
// Returns false if idx is out of bounds or the list is empty
bool hashMap_setCurrent(HASH_MAP_KEY key,int idx);

// Returns the index of the current element
// Note: returns -1 if the list is empty
int hashMap_getCurrentIdx(HASH_MAP_KEY key);

// Returns the number of elements currently in the list
int hashMap_getSize(HASH_MAP_KEY key);

//////////// Iterator functions to maintain the modularity according to songManager's needs ////////////

// Sets list's position to be displayed from to the Head
// Note: Returns false if the list is empty
bool hashMap_setIteratorStartPosition(HASH_MAP_KEY key);

// Sets list's position to be displayed from to the Tail
bool hashMap_setIteratorEndPosition(HASH_MAP_KEY key);

// Moves the display one position to the right
// Note: Returns false if the list is empty or there is no next element
bool hashMap_iteratorNext(HASH_MAP_KEY key);

// Moves the display one position to the left and returns the data at the that position
// Note: Returns false if the list is empty or there is no previous element
bool hashMap_iteratorPrev(HASH_MAP_KEY key);

// Resets the current pointer to point to the element
// Returns false if idx is out of bounds or the list is empty
// TODO: Remove??
bool hashMap_setIterator(HASH_MAP_KEY key,int idx);

// Returns the data of the "currentDisplay"
void *hashMap_getCurrentIteratorElement(HASH_MAP_KEY key);

// Moves the iterator "n" positions forward or to the last element if there aren't enough elements
// Returns false if the iterator cannot be moved
bool hashMap_advanceIteratorNTimes(HASH_MAP_KEY key, int n);

// Moves the iterator "n" positions backwards or to the last element if there aren't enough elements
// Returns false if the iterator cannot be moved
bool hashMap_rewindIteratorNTimes(HASH_MAP_KEY key,int n);

#endif // HASH_MAP_H
