#define _GNU_SOURCE
#include "list.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define UNUSED(x) ((x) = (x))

#define assert(p) ((p) ? my_str("\x1B[32m✓\x1B[0m " #p "\n") : assert_failed(#p, __FILE__, __LINE__))
void assert_failed(char *p, char *file, int line) {
    my_str(file);
    my_char(':');
    my_int(line);
    my_str(": Assertion `");
    my_str(p);
    my_str("' failed\n");
    kill(getpid(), SIGABRT); // suicide
}

// test code that prints to stdout
#define PRTEST(code) { \
    my_str(#code); \
    my_str(": "); \
    code; \
    my_char('\n'); \
}

void note(char *s) {
    my_str("\x1B[33m");
    my_str(s);
    my_str("\x1B[0m");
}

int *mkint(int i) {
    int *p = malloc(sizeof(int));
    *p = i;
    return p;
}

int main()
{
    my_str("\n\n\n");
    struct s_node *head;
    int *i1 = mkint(1), *i2 = mkint(2), *i3 = mkint(3);

    note("NULL checks\n");
    head = new_node(NULL, NULL, NULL);
    add_node(NULL, &head);
    assert(head->next == NULL && head->prev == NULL);
    struct s_node *s = new_node(NULL, NULL, NULL);
    add_node(s, &head);
    remove_node(&s);
    assert(s == NULL);
    assert(head->next == NULL && head->prev == NULL);
    append(NULL, &head);
    assert(head->next == NULL && head->prev == NULL);
    s = new_node(NULL, NULL, NULL);
    append(s, &head);
    remove_node(&s);
    assert(head->next == NULL && head->prev == NULL);
    add_node_at(NULL, &head, 0);
    assert(head->next == NULL && head->prev == NULL);
    s = new_node(NULL, NULL, NULL);
    add_node_at(s, &head, 0);
    void *nullelem = remove_node(&s);
    assert(nullelem == NULL);
    assert(head->next == NULL && head->prev == NULL);

    traverse_int(head);
    debug_int(head);
    empty_list(&head);
    my_char('\n');

    empty_list(NULL);
    assert(node_at(NULL, 42) == NULL);
    assert(elem_at(NULL, 42) == NULL);
    assert(count_s_nodes(NULL) == 0);

    my_str("\n-------\n");

    head = new_node(i3, NULL, NULL);
    traverse_int(head);
    debug_int(head);
    assert(remove_node(&head) == i3);
    assert(head == NULL);

    my_str("-------\n");

    head = new_node(i1, NULL, NULL);
    add_elem(i2, &head);
    note("added 1, 2\n");
    assert(*(int*)head->elem == 2);
    traverse_int(head);
    debug_int(head);

    remove_node(&head);
    note("\nremoved head, now we have: ");
    debug_int(head);

    remove_node(&head);
    note("\nremoved head, now we have: ");
    debug_int(head);
    my_char('\n');

    assert(head == NULL);

    my_str("-------\n");

    head = new_node(i1, NULL, NULL);
    add_elem(i2, &head);
    note("added 1, 2\n");
    assert(*(int*)head->elem == 2);
    traverse_int(head);
    debug_int(head);

    remove_last(&head);
    note("\nremoved last, now we have: ");
    debug_int(head);

    remove_last(&head);
    note("\nremoved last, now we have: ");
    debug_int(head);
    my_char('\n');

    assert(head == NULL);

    my_str("-------\n");

    head = new_node(i1, NULL, NULL);
    add_elem(i2, &head);
    add_elem(i3, &head);
    note("added 1, 2, 3\n");
    assert(*(int*)head->elem == 3);
    traverse_int(head);
    debug_int(head);

    int *elem = remove_node_at(&head, 1);
    my_char('\n');
    assert(*elem == 2);
    note("\nremoved list[1], now we have: ");
    debug_int(head);

    elem = remove_node_at(&head, 0);
    my_char('\n');
    assert(*elem == 3);
    note("\nremoved list[0], now we have: ");
    debug_int(head);
    my_char('\n');

    elem = remove_last(&head);
    assert(*elem == 1);

    assert(head == NULL);

    empty_list(&head);

    my_str("-------\n");

    head = new_node(i1, NULL, NULL);
    add_elem(i2, &head);
    add_elem(i3, &head);
    note("added 1, 2, 3\n");
    assert(*(int*)head->elem == 3);
    traverse_int(head);
    debug_int(head);

    empty_list(&head);
    note("\ndeleted list: ");
    debug_int(head);

    my_str("\n-------\n");

    head = new_node("head", NULL, NULL);
    add_elem("new-head", &head);
    append(new_node("new-tail", NULL, NULL), &head);
    assert(my_strcmp(head->elem, "new-head") == 0);
    traverse_string(head);
    my_char('\n');
    add_node_at(new_node("index-1", NULL, NULL), &head, 1);
    add_node_at(new_node("last-node", NULL, NULL), &head, 99);
    add_node_at(new_node("index-3", NULL, NULL), &head, 3);
    add_node_at(NULL, &head, 2);
    struct s_node *null_node = new_node(NULL, NULL, NULL);
    add_node_at(null_node, &head, 2);
    remove_node(&null_node);
    traverse_string(head);
    debug_string(head);
    empty_list(&head);

    my_str("\n-------\n");

    head = new_node("head", NULL, NULL);
    add_elem("new-head", &head);
    struct s_node *next = node_at(head, 1);
    append(new_node("new-tail", NULL, NULL), &head);
    traverse_string(head);
    remove_node(&head);
    assert(head == next);
    empty_list(&head);

    my_str("\n-------\n");

    char *str = "helloworld";
    head = new_node(&str[2], NULL, NULL);
    assert(head->elem == &str[2]);
    assert(head->next == NULL);
    assert(head->prev == NULL);
    assert(count_s_nodes(head) == 1);
    add_elem(&str[1], &head);
    assert(count_s_nodes(head) == 2);
    add_elem(&str[0], &head);
    assert(count_s_nodes(head) == 3);
    struct s_node *node_L = new_node(&str[3], NULL, NULL);
    append(node_L, &head);
    assert(count_s_nodes(head) == 4);
    append(new_node(&str[4], NULL, NULL), &head);
    assert(count_s_nodes(head) == 5);
    append(new_node(&str[5], NULL, NULL), &head);
    assert(count_s_nodes(head) == 6);
    struct s_node *badnode = new_node("bad node", NULL, NULL);
    add_node_at(badnode, &head, 3);
    assert(count_s_nodes(head) == 7);
    char *badelem = remove_node(&badnode);
    assert(my_strcmp(badelem, "bad node") == 0);
    assert(count_s_nodes(head) == 6);
    traverse_char(head);
    debug_char(head);
    my_char('\n');
    char *l_elem = remove_last(&head);
    assert(*l_elem == 'w');
    assert(count_s_nodes(head) == 5);
    traverse_char(head);
    my_char('\n');
    l_elem = remove_node_at(&head, 1);
    assert(count_s_nodes(head) == 4);
    my_char('\n');
    debug_char(head);
    my_char('\n');
    assert(node_at(head, 2) == node_L);
    assert(*(char*)node_L->elem == 'l');
    assert(elem_at(head, 2) == node_L->elem);
    empty_list(&head);
    assert(count_s_nodes(head) == 0);

    // make sure nothing changed these
    assert(*i1 == 1);
    assert(*i2 == 2);
    assert(*i3 == 3);
    free(i1);
    free(i2);
    free(i3);

    my_str("\n\n\n");
}
