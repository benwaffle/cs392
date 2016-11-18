#include "list.h"

void add_node(struct s_node *node, struct s_node **head)
{
    add_node_at(node, head, 0);
}
