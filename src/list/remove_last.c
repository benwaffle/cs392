#include "list.h"

void *remove_last(struct s_node **head)
{
    if (head == NULL || *head == NULL)
        return NULL;

    struct s_node **last = head;
    while ((*last)->next != NULL)
        last = &(*last)->next;

    return remove_node(last);
}
