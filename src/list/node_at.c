#include "list.h"

struct s_node *node_at(struct s_node *head, int n)
{
    while (head != NULL && n > 0) {
        head = head->next;
        --n;
    }
    return head;
}
