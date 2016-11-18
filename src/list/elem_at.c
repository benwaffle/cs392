#include "list.h"

void *elem_at(struct s_node *head, int n)
{
    if (head == NULL)
        return NULL;
    return node_at(head, n)->elem;
}
