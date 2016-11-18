#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct block {
    struct block *next;
    size_t size;
    void *mem; // is this redundant
} block;

block *head = NULL;
block *tail = NULL;

void print_blocks()
{
    printf("\t");
    for (block *b = head; b != NULL; b = b->next) {
        printf("block { size = %ld, mem = %p } -> ", b->size,
               (void *)((char *)b->mem + sizeof(block)));
        fflush(stdout);
    }
    printf("\b \b\b \b\b \b\n---------\n");
}

void add_node(block *b)
{
    if (head == NULL && tail == NULL) {
        head = tail = b;
    } else {
        tail->next = b;
        tail = b;
    }
}

void remove_node(block *n)
{
    if (head == NULL)
        return;
    if (n == head && n == tail) {
        head = tail = NULL;
        return;
    }
    if (n == head) {
        head = n->next;
        return;
    }

    block *b; // node before n
    for (b = head; b->next != n; b = b->next)
        ;

    b->next = n->next;
}

void *my_malloc(size_t size)
{
    if (head != NULL) {
        // look for a free gap
        block *prev = NULL;
        for (block *b = head; b->next != NULL; b = b->next) {
            char *endblock = (char *)b->mem + b->size + sizeof(block);
            char *beginnext = b->next->mem;
            if (endblock - beginnext >= size + sizeof(block)) {
                prev = b;
                break;
            }
        }

        // if we have a gap, use it
        if (prev != NULL) {
            printf("Got a gap!\n");
            void *mem = (char *)prev->mem + prev->size + sizeof(block);
            // clang-format off
            *(block *)mem = (block) {
                .next = prev->next,
                .size = size,
                .mem = mem,
            };
            // clang-format on
            prev->next = (block *)mem;
            return (char *)mem + sizeof(block);
        }
    }

    // else increase data segment

    void *mem = sbrk(size + sizeof(block));

    if (mem == (void *)-1) {
        perror("sbrk");
        exit(1);
    }

    // clang-format off
    *(block *)mem = (block) {
        .size = size,
        .mem = mem,
    };
    // clang-format on

    add_node(mem);

    return (char *)mem + sizeof(block);
}

void my_free(void *mem)
{
    block *b = (void *)((char *)mem - sizeof(block));
    printf("freeing %p, ", b->mem);

    if (b == tail) {
        void *curbrk = sbrk(0);
        brk((char *)curbrk - sizeof(block) - b->size);
        printf("new break = %p\n", sbrk(0));
    } else {
        printf("\n");
    }

    remove_node(b);
}

// be smarter to fit in current space
void *my_realloc(void *mem, size_t sz)
{
    block *b = (void *)((char *)mem - sizeof(block));
    void *new = my_malloc(sz); // new size;
    for (int i = 0; i < sz && i < b->size; ++i)
        ((char *)new)[i] = ((char *)mem)[i];
    my_free(mem);
    return new;
}

int main()
{
    void *a = my_malloc(5);
    void *b = my_malloc(5);
    void *c = my_malloc(5);
    printf("allocated 3 x 5 bytes\n");
    print_blocks();

    my_free(b); // free a gap
    printf("freed middle\n");
    print_blocks();

    printf("allocating a block that can go in the gap\n");
    void *d = my_malloc(3);
    assert(b == d);
    print_blocks();

    printf("allocating 10\n");
    void *e = my_malloc(10);
    print_blocks();

    printf("realloc\n");
    my_realloc(d, 10);

    a = a;
    c = c;
    e = e;

    print_blocks();
}
