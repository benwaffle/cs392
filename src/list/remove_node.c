#include "list.h"

void *remove_node(struct s_node **node)
{
    if (node == NULL || *node == NULL)
        return NULL;

    // create a copy of the node so we don't overwrite it
    struct s_node *mynode = *node;

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
