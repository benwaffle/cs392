#include "list.h"

void traverse_int(struct s_node *head)
{
    for (struct s_node *n = head; n != NULL; n = n->next) {
        print_int(n);
        my_char(' ');
    }
}
