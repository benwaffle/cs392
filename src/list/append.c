#include "list.h"

void append(struct s_node *node, struct s_node **head)
{
    if (head == NULL) // invalid
        return;

    if (node == NULL || node->elem == NULL) // null node
        return;

    if (*head == NULL) { // empty list
        *head = node;
        return;
    }

    struct s_node *last = *head;
    while (last->next != NULL)
        last = last->next;
    last->next = node;
    node->prev = last;
}
