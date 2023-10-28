#include "../include/list.h"
#include <string.h>

void list_init(List* list, void (*destroy)(void* data)) {
    list->size = 0;
    list->destroy = destroy;
    list->head = NULL;
    list->tail = NULL;
}

void list_destroy(List* list) {
    void* data;

    /* Remove all elements */
    while (list_size(list)) {
        if (!list_rem_next(list, NULL, (void**) &data) && list->destroy) {
            list->destroy(data);
        }
    }

    memset(list, 0, sizeof(List));
}

int list_ins_next(List* list, ListElmt* element, const void* data) {
    ListElmt* new_element;

    /* Allocate storage for the element */
    new_element = (ListElmt*) malloc(sizeof(ListElmt));
    if (!new_element) {
        return -1;
    }

    new_element->data = (void*) data;
    if (!element) {
        /* Handle insertion at the head of the list */
        if (!list_size(list)) {
            list->tail = new_element;
        }

        new_element->next = list->head;
        list->head = new_element;
    }
    else {
        /* Handle insertion somewhere other than the head */
        if (!element->next) {
            list->tail = new_element;
        }

        new_element->next = element->next;
        element->next = new_element;
    }

    /* Adjust the size of the list */
    list->size += 1;

    return 0;
}

int list_rem_next(List* list, ListElmt* element, void** data) {
    ListElmt* old_element;

    /* Do not allow removal from an empty list */
    if (!list_size(list)) {
        return -1;
    }

    /* Remove the element from the list */
    if (!element) {
        /* Handle removal from the head of the list */
        *data = list->head->data;
        old_element = list->head;
        list-> head = list->head->next;

        if (list_size(list) == 1) {
            list->tail = NULL;
        }
    }
    else {
        /* Handle removal from somewhere other than the head */
        if (!element->next) {
            return -1;
        }

        *data = element->next->data;
        old_element = element->next;
        element->next = element->next->next;

        if (!element->next) {
            list->tail = element;
        }
    }

    /* Free the storage allocated by the abstract data type */
    free(old_element);

    /* Adjust the size of the list */
    list->size -= 1;

    return 0;
}
