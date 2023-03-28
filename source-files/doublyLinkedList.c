
/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Declarations of a general doublyLinkedList
Use cases: 1) Store the name of the songs
Author: Amirhossein Etaati
Date: 2023-03-16
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "doublyLinkedList.h"

struct Node
{
    void *data;
    struct Node *next;
    struct Node *prev;
};

struct List
{
    struct Node *head;
    struct Node *tail;

    struct Node *current; // keeps track of the current element in the list
    int curentIdx;        // TODO: -1 if there is no element in the list
};

static bool is_module_initialized = false;
static struct List *list_ptr = NULL;

static void push_to_head(void *src, unsigned int size)
{
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_head(): Error - There was a problem allocating memory.");
        exit(1);
    }
    (new_node->data) = malloc(size);
    if ((new_node->data) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_head(): Error - There was a problem allocating memory for t");
        exit(1);
    }

    memcpy((new_node->data), (src), size);
    (new_node->next) = (list_ptr->head);
    (new_node->prev) = NULL;

    if (list_ptr->head != NULL)
    {
        list_ptr->head->prev = new_node;
        (list_ptr->head) = new_node;
    }
    else
    {
        // List was previously empty
        list_ptr->tail = list_ptr->head = list_ptr->current = new_node;
        list_ptr->curentIdx = 0;
    }
}

static void push_to_tail(void *src, unsigned int size)
{
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_tail(): Error - There was a problem allocating memory.");
        exit(1);
    }
    (new_node->data) = malloc(size);
    if ((new_node->data) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_tail(): Error - There was a problem allocating memory for t");
        exit(1);
    }

    memcpy((new_node->data), (src), size);
    (new_node->next) = NULL;
    (new_node->prev) = list_ptr->tail;

    if (list_ptr->tail != NULL)
    {
        (list_ptr->tail->next) = new_node;
        list_ptr->tail = new_node;
    }
    else
    {
        list_ptr->tail = list_ptr->head = list_ptr->current = new_node;
        list_ptr->curentIdx = 0;
    }
}

static void pop_from_head(void)
{
    if ((list_ptr->head) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_pop_from_head(): Error - The list is empty");
        exit(1);
    }

    struct Node *first_node = (list_ptr->head);
    (list_ptr->head) = list_ptr->head->next;
    if (list_ptr->head != NULL)
    {
        list_ptr->head->prev = NULL;
    }
    free(first_node->data);
    free(first_node);
}

////////////////////////////////////////// Public Function //////////////////////////////////////////

void doublyLinkedList_init(void)
{
    list_ptr = (struct List *)malloc(sizeof(struct List));
    list_ptr->head = NULL;
    list_ptr->tail = NULL;
    list_ptr->current = NULL;
    list_ptr->curentIdx = -1;

    is_module_initialized = true;
}

// // TODO: remove
// struct Node *doublyLinkedList_getHead(void)
// {
//     return list_ptr->head;
// }

bool doublyLinkedList_isEmpty(void)
{
    assert(is_module_initialized);
    return (!list_ptr->head || !list_ptr->tail);
}

void doublyLinkedList_appendItem(void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_tail(src, size);
}

void doublyLinkedList_prependItem(void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_head(src, size);
}

bool doublyLinkedList_next(void)
{
    assert(is_module_initialized);
    if (list_ptr->current == NULL || list_ptr->current->next == NULL)
    {
        return false;
    }
    list_ptr->current = list_ptr->current->next;
    list_ptr->curentIdx += 1;
    return true;
}

bool doublyLinkedList_prev(void)
{
    assert(is_module_initialized);
    if (list_ptr->current == NULL || list_ptr->current->prev == NULL)
    {
        return false;
    }
    list_ptr->current = list_ptr->current->prev;
    list_ptr->curentIdx -= 1;
    return true;
}

void *doublyLinkedList_getCurrentElement(void)
{
    assert(is_module_initialized);
    if (!doublyLinkedList_isEmpty() && list_ptr->current != NULL)
    {
        return list_ptr->current->data;
    }
    return NULL;
}

void doublyLinkedList_cleanup(void)
{
    assert(is_module_initialized);
    while (!doublyLinkedList_isEmpty())
    {
        pop_from_head();
    }
    free(list_ptr);
}

//////////// Extra functions to maintain the modularity according to songManager's needs ////////////

void *doublyLinkedList_getElementAtIndex(int idx)
{
    assert(is_module_initialized);
    if (idx < 0)
        return NULL;

    int counter = 0;
    struct Node *node = list_ptr->head;

    while (counter < idx && node != NULL)
    {
        node = node->next;
        counter++;
    }
    if (node == NULL)
        return NULL;
    return node->data;
}

// Sets the current pointer to point to the element at index "idx"
// Returns false if idx is out of bounds or the list is empty, true if successful
bool doublyLinkedList_setCurrent(int idx)
{
    assert(is_module_initialized);
    if (idx < 0)
        return false;

    int counter = 0;
    struct Node *node = list_ptr->head;

    while (counter < idx && node != NULL)
    {
        node = node->next;
        counter++;
    }
    if (node == NULL)
        return false;

    list_ptr->current = node;
    list_ptr->curentIdx = idx;
    return true;
}

// Returns the index of the current element
// Note: returns -1 if the list is empty
int doublyLinkedList_getCurrentIdx()
{
    assert(is_module_initialized);
    if (list_ptr->head == NULL)
    {
        return -1;
    }
    return list_ptr->curentIdx;
}

// Returns the number of elements currently in the list
int doublyLinkedList_getSize()
{
    assert(is_module_initialized);
    return doublyLinkedList_getCurrentIdx() + 1;
}

/**********************************************************************/
// int main(int argc, char const *argv[])
// {
//     doublyLinkedList_init();

//     doublyLinkedList_appendItem("Hello1", strlen("Hello1") + 1);
//     doublyLinkedList_appendItem("Hello2", strlen("Hello2") + 1);
//     doublyLinkedList_appendItem("Hello3", strlen("Hello3") + 1);

//     char *result = doublyLinkedList_getElementAtIndex(3);
//     printf("==> the result is: %s\n", result);

//     if (doublyLinkedList_setCurrent(2)) {
//         printf("setCurrent was successful - current is: %s\n", doublyLinkedList_getCurrentElement());
        
//     }
//     else {
//         printf("Error - setCurrent was unsuccessful - current is: %s\n", doublyLinkedList_getCurrentElement());
//     }


//     do
//     {
//         if (!doublyLinkedList_isEmpty())
//         {
//             printf("======> %s\n", (char *)doublyLinkedList_getCurrentElement());
//         }
//     } while (doublyLinkedList_next());

//     do
//     {
//         if (!doublyLinkedList_isEmpty())
//         {
//             printf("======> %s\n", (char *)doublyLinkedList_getCurrentElement());
//         }
//     } while (doublyLinkedList_prev());

//     doublyLinkedList_cleanup();
//     return 0;
// }
