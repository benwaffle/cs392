#include "list.h"

void debug_string(struct s_node* head)
{
    for (struct s_node *n = head; n != NULL; n = n->next) {
        my_char('(');
        print_string(n->prev);
        my_str(" <- ");
        print_string(n);
        my_str(" -> ");
        print_string(n->next);
        my_char(')');

        if (n->next != NULL)
            my_str(", ");
    }
}
