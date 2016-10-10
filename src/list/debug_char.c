#include "list.h"

void debug_char(struct s_node* head)
{
    for (struct s_node *n = head; n != NULL; n = n->next) {
        my_char('(');
        print_char(n->prev);
        my_str(" <- ");
        print_char(n);
        my_str(" -> ");
        print_char(n->next);
        my_char(')');

        if (n->next != NULL)
            my_str(", ");
    }
}
