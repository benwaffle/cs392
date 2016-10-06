#include "list.h"

int count_s_nodes(struct s_node* head)
{
    int i = 0;
    while (head != NULL) {
        ++i;
        head = head->next;
    }
    return i;
}

struct s_node* node_at(struct s_node* head, int n)
{
    while (head != NULL && n > 0) {
        head = head->next;
        --n;
    }
    return head;
}

void* elem_at(struct s_node* head, int n)
{
    if (head == NULL)
        return NULL;
    return node_at(head, n)->elem;
}
