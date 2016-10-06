#include "list.h"

void* remove_node(struct s_node** node)
{
    if (node == NULL || *node == NULL)
        return NULL;

    struct s_node *mynode = *node; // create a copy of the node so we don't overwrite it

    if (mynode->prev != NULL)
        mynode->prev->next = mynode->next;
    if (mynode->next != NULL)
        mynode->next->prev = mynode->prev;

    void *elem = mynode->elem;

    if (mynode->prev == NULL && mynode->next == NULL) { // list is empty
        free(mynode);
        *node = NULL; // TODO: should we always do this
    } else if (mynode->prev == NULL) { // remove head
        struct s_node *next = mynode->next;
        free(mynode);
        *node = next;
    } else {
        free(mynode);
    }

    return elem;
}

void* remove_last(struct s_node** head)
{
    if (head == NULL || *head == NULL)
        return NULL;

    struct s_node **last = head;
    while ((*last)->next != NULL)
        last = &(*last)->next;

    return remove_node(last);
}

void* remove_node_at(struct s_node** head, int n)
{
    if (head == NULL || *head == NULL)
        return NULL;

    struct s_node **loc = head;

    while ((*loc)->next != NULL && n > 0) {
        --n;
        loc = &(*loc)->next;
    }

    return remove_node(loc);
}

void empty_list(struct s_node** head)
{
    if (head == NULL)
        return;
    while (*head)
        remove_node(head);
}
