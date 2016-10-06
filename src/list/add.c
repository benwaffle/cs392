#include "list.h"

void add_node(struct s_node* node, struct s_node** head)
{
    if (head == NULL) // invalid
        return;

    if (node == NULL || node->elem == NULL) // null node
        return;

    if (*head == NULL) { // empty list
        *head = node;
        return;
    }

    node->next = *head;
    (*head)->prev = node;
    *head = node;
}

void add_elem(void* elem, struct s_node** head)
{
    add_node(new_node(elem, NULL, NULL), head);
}

void add_node_at(struct s_node* node, struct s_node** head, int n)
{
    if (head == NULL) // invalid
        return;

    if (node == NULL || node->elem == NULL) // null node
        return;

    if (*head == NULL) { // empty list
        *head = node;
        return;
    }

    struct s_node *loc = *head;

    while (loc->next != NULL && n > 0) {
        --n;
        loc = loc->next;
    }

    if (loc->next == NULL) { // tail
        loc->next = node;
        node->prev = loc;
    } else {
        node->next = loc;
        node->prev = loc->prev;
        loc->prev->next = node;
        loc->prev = node;
    }
}
