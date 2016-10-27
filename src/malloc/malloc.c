#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>

void *my_malloc(size_t size) {
    return sbrk(size);
}

int main() {
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(100));
    printf("%p\n", my_malloc(5));
    printf("%p\n", my_malloc(5));
}
