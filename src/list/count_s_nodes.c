#include "list.h"

int count_s_nodes(struct s_node *head)
{
    int i = 0;
    while (head != NULL) {
        ++i;
        head = head->next;
    }
    return i;
}
