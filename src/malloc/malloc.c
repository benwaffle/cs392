#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct block {
    struct block *next;
    size_t size;
    void *mem;
} block;

block *head = NULL;
block *tail = NULL;

void print_blocks() {
    printf("\t");
    for (block *b = head; b != NULL; b = b->next) {
        printf("block { size = %ld, mem = %p } -> ", b->size, (void*)((char*)b->mem + sizeof(block)));
    }
    printf("\b \b\b \b\b \b\n---------\n");
}

void add_node(block *b) {
    if (head == NULL && tail == NULL) {
        head = tail = b;
    } else {
        tail->next = b;
        tail = b;
    }
}

void remove_last() {
    if (head == NULL)
        return;
    block *b;
    for (b = head; b->next != tail; b = b->next)
        ;
    if (b == head) {
        head = tail = NULL;
    } else {
        tail = b;
        tail->next = NULL;
    }
}

void *my_malloc(size_t size) {
    void *mem = sbrk(size + sizeof(block));

    if (mem == (void*)-1) {
        perror("sbrk");
        exit(1);
    }

    *(block*)mem = (block){
        .size = size,
        .mem = mem,
    };

    add_node(mem);

    return (char*)mem + sizeof(block);
}

void my_free(void *mem) {
    block *b = (void*)((char*)mem - sizeof(block));
    printf("freeing %p, ", b->mem);

    remove_last();
    void *curbrk = sbrk(0);
    brk((char*)curbrk - sizeof(block) - b->size);
    printf("new break = %p\n", sbrk(0));
}

int main() {
    printf("Allocating a bunch of stuff:\n");
    {
        void *m = my_malloc(5);
        printf("\tmalloc -> %p\n", m);
//        my_free(m);
    }
    print_blocks();
    {
        void *m = my_malloc(5);
        printf("\tmalloc -> %p\n", m);
 //       my_free(m);
    }
    print_blocks();
    {
        void *m = my_malloc(5);
        printf("\tmalloc -> %p\n", m);
  //      my_free(m);
    }
    print_blocks();
    {
        void *m = my_malloc(5);
        printf("\tmalloc -> %p\n", m);
        my_free(m);
    }
    print_blocks();
    {
        void *m = my_malloc(100);
        printf("\tmalloc -> %p\n", m);
        my_free(m);
    }
    print_blocks();
    {
        void *m = my_malloc(5);
        printf("\tmalloc -> %p\n", m);
        my_free(m);
    }

    printf("\nTraversing the list:\n");
    print_blocks();
}
