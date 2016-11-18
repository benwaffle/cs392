#include "list.h"

void *remove_node_at(struct s_node **head, int n)
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
