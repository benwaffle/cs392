#include "list.h"

struct s_node *new_node(void *elem, struct s_node *next, struct s_node *prev)
{
    struct s_node *n = malloc(sizeof(struct s_node));
    n->elem = elem;
    n->prev = prev;
    n->next = next;
    return n;
}
