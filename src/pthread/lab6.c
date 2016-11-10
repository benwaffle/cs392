// Compute the sum of a file full of binary 16-bit integer data,
// parallel version.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct mmap_region {
    int fd;
    void *addr;
    size_t size;
};

struct thread_data {
    uint16_t *addr;
    size_t len;
    int *sum;
};

size_t get_file_size(const char *filename);
void map_file_region(const char *filename, size_t size, struct mmap_region *region);
void unmap_file_region(struct mmap_region *region);

void* calc_sum(void *_data) {
    struct thread_data *data = _data;
    for (int i=0; i<data->len; ++i)
        *data->sum += data->addr[i]; 
    return NULL;
}

int main(int argc, char **argv)
{
    uint16_t *data;
    size_t i, size, num_elements;
    uint64_t sum;
    struct mmap_region region;

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s <input file> [number of threads]\n", argv[0]);
        exit(1);
    }

    int nthreads = 2;
    if (argc == 3)
        nthreads = atoi(argv[2]);
    int *sums = calloc(nthreads, sizeof(int));
    printf("Using %d threads\n", nthreads);

    // Find out how large the file is.
    size = get_file_size(argv[1]);
    printf("File is %lu bytes in size\n", size);

    // Each element (datum) is 16 bits.
    // Find out how many elements there are overall.
    num_elements = size / sizeof(uint16_t);

    // Map the file data into memory
    map_file_region(argv[1], size, &region);
    data = region.addr;

    pthread_t threads[nthreads];
    struct thread_data tdata[nthreads];
    int nperthread = num_elements / nthreads;

    for (int i=0; i<nthreads; ++i) {
        int len = nperthread;
        if (i == nthreads-1) {
            len = num_elements - nperthread * (nthreads-1);
        }
        tdata[i] = (struct thread_data) {
            .addr = data + nperthread * i,
            .len = len,
            .sum = &sums[i]
        };
        pthread_create(&threads[i], NULL, calc_sum, &tdata[i]);
    }

    for (int i=0; i<nthreads; ++i)
        pthread_join(threads[i], NULL);

    // Compute the sum
    sum = 0ULL;
    for (i = 0; i < nthreads; i++) {
        sum += sums[i];
    }

    printf("Sum is %lu\n", sum);

    unmap_file_region(&region);

    return 0;
}

size_t get_file_size(const char *filename)
{
    struct stat file_info;

    if (stat(filename, &file_info) < 0) {
        fprintf(stderr, "Could not stat %s: %s\n", filename, strerror(errno));
        exit(1);
    }
    return (size_t)file_info.st_size;
}

void map_file_region(const char *filename, size_t size, struct mmap_region *region)
{
    region->size = size;

    // Open the file
    region->fd = open(filename, O_RDONLY);
    if (region->fd < 0) {
        fprintf(stderr, "Could not open %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    // Map the file into memory.
    region->addr = mmap(NULL, (size_t)size, PROT_READ, MAP_PRIVATE, region->fd, (off_t)0);
    if (region->addr == MAP_FAILED) {
        fprintf(stderr, "Could not map %s: %s\n", filename, strerror(errno));
        exit(1);
    }
}

void unmap_file_region(struct mmap_region *region)
{
    munmap(region->addr, region->size);
    close(region->fd);
}


