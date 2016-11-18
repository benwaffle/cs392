#include "list.h"

void traverse_string(struct s_node *head)
{
    for (struct s_node *n = head; n != NULL; n = n->next) {
        print_string(n);
        my_char(' ');
    }
}
