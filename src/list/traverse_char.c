#include "list.h"

void traverse_char(struct s_node* head)
{
    for (struct s_node *n = head; n != NULL; n = n->next) {
        print_char(n);
        my_char(' ');
    }
}
