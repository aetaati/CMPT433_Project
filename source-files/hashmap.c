
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "hashmap.h"
#define MAX_NUM_BUCKETS 3

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
    int curentIdx;        // -1 if there is no element in the list

    struct Node *currentDisplay; // pointer to display the elements without changing "current" - TODO: change name to currentIterator

    int size;
};

struct HashMap
{
    struct List* list_ptr;
    HASH_MAP_KEY key;
};

static bool is_module_initialized = false;
//static struct List *list_ptr = NULL;
static struct HashMap hash_map[MAX_NUM_BUCKETS];

static void push_to_head(struct List *list_ptr, void *src, unsigned int size)
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
        list_ptr->tail = list_ptr->head = list_ptr->current = list_ptr->currentDisplay = new_node;
        list_ptr->curentIdx = 0;
    }

    list_ptr->size += 1;
}

static void push_to_tail(struct List *list_ptr, void *src, unsigned int size)
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
        list_ptr->tail = list_ptr->head = list_ptr->current = list_ptr->currentDisplay = new_node;
        list_ptr->curentIdx = 0;
    }

    list_ptr->size += 1;
}

static void pop_from_head(struct List *list_ptr)
{
    if ((list_ptr->head) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_pop_from_head(): Error - The list is empty");
        exit(1);
    }

    if (list_ptr->current == list_ptr->head)
    {
        list_ptr->current = list_ptr->head->next; // if current is pointing to head, set it to the next element
    }
    if (list_ptr->currentDisplay == list_ptr->head)
    {
        list_ptr->currentDisplay = list_ptr->head->next; // if currentDisplay is pointing to head, set it to the next element
    }

    struct Node *first_node = (list_ptr->head);
    (list_ptr->head) = list_ptr->head->next;
    if (list_ptr->head != NULL)
    {
        list_ptr->head->prev = NULL;
    }
    free(first_node->data);
    free(first_node);

    list_ptr->size -= 0;
}

static bool set_ptr_to_idx(struct List *list_ptr, int idx, struct Node *ptr)
{
    int counter = 0;
    struct Node *node = list_ptr->head;

    while (counter < idx && node != NULL)
    {
        node = node->next;
        counter++;
    }
    if (node == NULL)
        return false;

    if (ptr == list_ptr->current)
    {
        list_ptr->curentIdx = idx;
    }
    ptr = node;
    return true;
}

////////////////////////////////////////// Public Function //////////////////////////////////////////

void hashMap_init(void)
{

    for(int i=0; i < NUM_KEYS_LIST; i++) {
        hash_map[i].list_ptr = (struct List *)malloc(sizeof(struct List));
        hash_map[i].list_ptr->head = NULL;
        hash_map[i].list_ptr->tail = NULL;
        hash_map[i].list_ptr->current = NULL;
        hash_map[i].list_ptr->curentIdx = -1;
        hash_map[i].list_ptr->currentDisplay = NULL;
        hash_map[i].list_ptr->size = 0;
    }

    is_module_initialized = true;
}

static bool doublyLinkedList_isEmpty(struct List *list_ptr)
{
    assert(is_module_initialized);
    return (!list_ptr->head || !list_ptr->tail);
}

static void doublyLinkedList_appendItem(struct List *list_ptr, void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_tail(list_ptr,src, size);
}

static void doublyLinkedList_prependItem(struct List *list_ptr, void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_head(list_ptr, src, size);
}

static bool doublyLinkedList_next(struct List *list_ptr)
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

static bool doublyLinkedList_prev(struct List *list_ptr)
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

static void *doublyLinkedList_getCurrentElement(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (!doublyLinkedList_isEmpty(list_ptr) && list_ptr->current != NULL)
    {
        return list_ptr->current->data;
    }
    return NULL;
}

static void doublyLinkedList_cleanup(struct List *list_ptr)
{
    assert(is_module_initialized);
    while (!doublyLinkedList_isEmpty(list_ptr))
    {
        pop_from_head(list_ptr);
    }
    free(list_ptr);
}

static void *doublyLinkedList_getElementAtIndex(struct List *list_ptr, int idx)
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
static bool doublyLinkedList_setCurrent(struct List *list_ptr, int idx)
{
    assert(is_module_initialized);
    return set_ptr_to_idx(list_ptr, idx, list_ptr->current);
}

// Returns the index of the current element
// Note: returns -1 if the list is empty
static int doublyLinkedList_getCurrentIdx(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->head == NULL)
    {
        return -1;
    }
    return list_ptr->curentIdx;
}

// Returns the number of elements currently in the list
static int doublyLinkedList_getSize(struct List *list_ptr)
{
    assert(is_module_initialized);
    return list_ptr->size;
}

//////////// Extra functions to maintain the modularity according to songManager's needs ////////////
/////////////////////////////////////////////////////////////////////////////

// Sets list's position to be displayed from to the Head
// Note: Returns false if the list is empty
static bool doublyLinkedList_setIteratorStartPosition(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->head == NULL)
    {
        return false;
    }
    list_ptr->currentDisplay = list_ptr->head;
    return true;
}

// Sets list's position to be displayed from to the Tail
static bool doublyLinkedList_setIteratorEndPosition(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->tail == NULL)
    {
        return false;
    }
    list_ptr->currentDisplay = list_ptr->tail;
    return true;
}

// Moves the display one position to the right
// Note: Returns false if the list is empty or there is no next element
static bool doublyLinkedList_iteratorNext(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->size == 0)
    {
        return false;
    }
    if ((list_ptr->currentDisplay)->next != NULL)
    {
        list_ptr->currentDisplay = (list_ptr->currentDisplay)->next;
        return true;
    }
    return false;
}

// Moves the display one position to the left and returns the data at the that position
// Note: Returns false if the list is empty or there is no previous element
static bool doublyLinkedList_iteratorPrev(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->size == 0)
    {
        return false;
    }
    if ((list_ptr->currentDisplay)->prev != NULL)
    {
        list_ptr->currentDisplay = (list_ptr->currentDisplay)->prev;
        return true;
    }
    return false;
}

// Returns the data of the "currentDisplay"
static void *doublyLinkedList_getCurrentIteratorElement(struct List *list_ptr)
{
    assert(is_module_initialized);
    if (list_ptr->currentDisplay == NULL)
    {
        return NULL;
    }
    return list_ptr->currentDisplay->data;
}

static bool doublyLinkedList_setIterator(struct List *list_ptr, int idx)
{
    assert(is_module_initialized);
    return set_ptr_to_idx(list_ptr, idx, list_ptr->currentDisplay);
}

static bool doublyLinkedList_advanceIteratorNTimes(struct List *list_ptr, int n)
{
    if (n <= 0 || list_ptr->tail == list_ptr->currentDisplay)
    {
        return false;
    }
    int counter = 0;
    while (counter < n && doublyLinkedList_iteratorNext(list_ptr))
    {
        counter++;
    }
    return true;
}

static bool doublyLinkedList_rewindIteratorNTimes(struct List *list_ptr, int n)
{
    if (n <= 0 || list_ptr->head == list_ptr->currentDisplay)
    {
        return false;
    }
    int counter = 0;
    while (counter < n && doublyLinkedList_iteratorPrev(list_ptr))
    {
        counter++;
    }
    return true;
}


bool hashMap_isEmpty(HASH_MAP_KEY key)
{
    return doublyLinkedList_isEmpty(hash_map[key].list_ptr);
}


void hashMap_prependItem(HASH_MAP_KEY key,void *src, unsigned int size)
{
    doublyLinkedList_prependItem(hash_map[key].list_ptr, src, size);
}

void hashMap_appendItem(HASH_MAP_KEY key,void *src, unsigned int size)
{
    doublyLinkedList_appendItem(hash_map[key].list_ptr, src, size);
}


bool hashMap_next(HASH_MAP_KEY key)
{
    return doublyLinkedList_next(hash_map[key].list_ptr);
}


bool hashMap_prev(HASH_MAP_KEY key){
    return doublyLinkedList_prev(hash_map[key].list_ptr);
}



void *hashMap_getElementAtIndex(HASH_MAP_KEY key,int idx)
{
   return doublyLinkedList_getElementAtIndex(hash_map[key].list_ptr, idx);
}


void *hashMap_getCurrentElement(HASH_MAP_KEY key)
{
    return doublyLinkedList_getCurrentElement(hash_map[key].list_ptr);

}

void hashMap_cleanup(void)
{
    for(int i=0; i < NUM_KEYS_LIST; i++) {
        doublyLinkedList_cleanup(hash_map[i].list_ptr);
    }
}

bool hashMap_setCurrent(HASH_MAP_KEY key,int idx)
{
    return doublyLinkedList_setCurrent(hash_map[key].list_ptr, idx);

}

int hashMap_getCurrentIdx(HASH_MAP_KEY key)
{
    return doublyLinkedList_getCurrentIdx(hash_map[key].list_ptr);
}

int hashMap_getSize(HASH_MAP_KEY key)
{
    return doublyLinkedList_getSize(hash_map[key].list_ptr);
}

bool hashMap_setIteratorStartPosition(HASH_MAP_KEY key)
{
    return doublyLinkedList_setIteratorStartPosition(hash_map[key].list_ptr);
}


bool hashMap_setIteratorEndPosition(HASH_MAP_KEY key)
{
    return doublyLinkedList_setIteratorEndPosition(hash_map[key].list_ptr);
}


bool hashMap_iteratorNext(HASH_MAP_KEY key)
{
    return doublyLinkedList_iteratorNext(hash_map[key].list_ptr);
}


bool hashMap_iteratorPrev(HASH_MAP_KEY key)
{
    return doublyLinkedList_iteratorPrev(hash_map[key].list_ptr);
}


bool hashMap_setIterator(HASH_MAP_KEY key,int idx)
{
    return doublyLinkedList_setIterator(hash_map[key].list_ptr, idx);
}


void *hashMap_getCurrentIteratorElement(HASH_MAP_KEY key)
{
    return doublyLinkedList_getCurrentIteratorElement(hash_map[key].list_ptr);
}


bool hashMap_advanceIteratorNTimes(HASH_MAP_KEY key, int n)
{
    return doublyLinkedList_advanceIteratorNTimes(hash_map[key].list_ptr, n);
}


bool hashMap_rewindIteratorNTimes(HASH_MAP_KEY key,int n)
{
    return doublyLinkedList_rewindIteratorNTimes(hash_map[key].list_ptr, n);
}


/**********************************************************************/
// int main(int argc, char const *argv[])
// {
//     doublyLinkedList_init();

//     doublyLinkedList_appendItem("Hello1", strlen("Hello1") + 1);
//     doublyLinkedList_appendItem("Hello2", strlen("Hello2") + 1);
//     doublyLinkedList_appendItem("Hello3", strlen("Hello3") + 1);

//     // printf("current size if %d\n", doublyLinkedList_getSize());

//     // char *result = doublyLinkedList_getElementAtIndex(3);
//     // printf("==> the result is: %s\n", result);

//     doublyLinkedList_advanceIteratorNTimes(3);
//     if (doublyLinkedList_rewindIteratorNTimes(300))
//     {
//         printf("==> current iterator element is: %s\n", doublyLinkedList_getCurrentIteratorElement());
//     }
//     else
//     {
//         printf("doublyLinkedList_advanceIteratorNTimes() failed\n");
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
