#include "list.h"

void empty_list(struct s_node** head)
{
    if (head == NULL)
        return;
    while (*head)
        remove_node(head);
}
