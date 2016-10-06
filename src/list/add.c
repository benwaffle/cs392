#include "list.h"

void add_node(struct s_node* node, struct s_node** head)
{
    add_node_at(node, head, 0);
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

    if (n == 0) { // add at head
        node->next = *head;
        (*head)->prev = node;
        *head = node; // new head
        return;
    }

    struct s_node *loc = *head;

    while (loc->next != NULL && n > 0) {
        --n;
        loc = loc->next;
    }

    if (n == 0) {
        node->next = loc;
        node->prev = loc->prev;
        if (loc->prev)
            loc->prev->next = node;
        loc->prev = node;
    } else { // tail
        loc->next = node;
        node->prev = loc;
    }
}
