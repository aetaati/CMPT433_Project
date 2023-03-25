/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Definitions of a general doublyLinkedList
Author: Amirhossein Etaati
Use cases: 1) Store the name of the songs
Date: 2023-03-16
*/

#if !defined(DOUBLY_LINKED_LIST_H)
#define DOUBLY_LINKED_LIST_H

// Returns a pointer to the newly allocated empty list
// Note: caller should call doublyLinkedList_cleanup() to free the memory
void doublyLinkedList_init(void);

// Returns true if the list is empty
bool doublyLinkedList_isEmpty(void);

// Adds the item "src" with size of "size" to the head of the list
void doublyLinkedList_prependItem(void *src, unsigned int size);

// Adds the item "src" with size of "size" to the tail of the list
void doublyLinkedList_appendItem(void *src, unsigned int size);

// Updates the "current" field of the list to the next node
// Returns true if update is successful, false if the list is empty
bool doublyLinkedList_next(void);

// Updates the "current" field of the list to the previous node
// Returns true if update is successful, false if the list is empty
bool doublyLinkedList_prev(void);

// Gets the data of the "current"
void *doublyLinkedList_getCurrentData(void);

// Frees the memory for all nodes, the data, and the List struct
void doublyLinkedList_cleanup(void);

#endif // DOUBLY_LINKED_LIST_H
