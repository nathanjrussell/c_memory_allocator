#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define CANARY_VALUE 0xDEADBEEF
#define CANARY_TYPE unsigned int
#define CANARY_SIZE sizeof(CANARY_TYPE)


// Structure representing a memory block
typedef struct Block {
    struct Block *next; // Pointer to the next block
    struct Block *prev; // Pointer to the previous block
    char is_free; // Flag to indicate if the block is free
    size_t size;
    size_t actual_size;
        const char* filename; // File name where memory was allocated
    int line; // Line number where memory was allocated
} Block;

// Global pointer to the allocated memory block
void *mem_start = NULL;
size_t MEMORY_SIZE = 0;


// Allocate memory using mmap for the memory block
void initialize_memory_pool(size_t size) {
    MEMORY_SIZE = size;
    // Map memory with anonymous mmap
    mem_start = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem_start == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
 
    // Initialize memory block structure
    Block *block = mem_start;
    block->is_free = 1;
    block->size = MEMORY_SIZE - sizeof(Block) - 2 * CANARY_SIZE;
    block->actual_size = block->size;

    block->next = NULL;
    block->prev = NULL;

}

void* mymalloc(size_t size, const char* filename, int line_number) {
    if (mem_start == NULL) {
        fprintf(stderr, "Memory not initialized\n");
        return NULL;
    }

    // Pointer to the start of the block
    Block *block = mem_start;

    // Find a suitable free space
    while (block != NULL && (block->size < size || !block->is_free)) {
        block = block->next;
    }
    
    if (block == NULL) {
        fprintf(stderr, "Not enough memory\n");
        return NULL;
    }

    // If the block is larger than the requested size, split it
    if (block->size > size + sizeof(Block) + 2 * CANARY_SIZE) {
        Block *new_block = (Block *)((char *)block + size + sizeof(Block)+ 2 * CANARY_SIZE);
        new_block->size = block->size - size - sizeof(Block) - 2 * CANARY_SIZE;
        new_block->actual_size = new_block->size;
        new_block->is_free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        if (block->next != NULL) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        block->actual_size = size;
    }
    block->size = size;
    CANARY_TYPE *canary1 = (CANARY_TYPE *)((char *)block + sizeof(Block));
    *canary1 = CANARY_VALUE;
    CANARY_TYPE *canary2 = (CANARY_TYPE *)((char *)block + sizeof(Block) + CANARY_SIZE + size);
    *canary2 = CANARY_VALUE;
    block->is_free = 0;
    

    // Return a pointer to the data portion of the block
    return (char *)block + sizeof(Block) + CANARY_SIZE;
}

void myfree(void *ptr) {
    if (ptr != NULL) {
        Block *block = (Block *)((char *)ptr - sizeof(Block) - CANARY_SIZE);
        CANARY_TYPE *canary1 = (CANARY_TYPE *)((char *)block + sizeof(Block));
        CANARY_TYPE *canary2 = (CANARY_TYPE *)((char *)block + sizeof(Block) + CANARY_SIZE + block->actual_size);
        if (*canary1 != CANARY_VALUE || *canary2 != CANARY_VALUE) {
            fprintf(stderr, "Memory corruption detected\n");
            return;
        }

        block->is_free = 1;
        block->size = block->actual_size;

        // Merge with next block if it is free
        if (block->next != NULL && block->next->is_free) {
            block->size += sizeof(Block) + CANARY_SIZE * 2 + block->next->size;
            block->actual_size = block->size;
            block->next = block->next->next;
            if (block->next != NULL) {
                block->next->prev = block;
            }
        }

        // Merge with previous block if it is free
        if (block->prev != NULL && block->prev->is_free) {
            block->prev->size += sizeof(Block) + CANARY_SIZE * 2 + block->size;
            block->prev->actual_size = block->prev->size;
            block->prev->next = block->next;
            if (block->next != NULL) {
                block->next->prev = block->prev;
            }
        }
    }
}

// Cleanup memory block using munmap
void destroy_memory_pool(){
    if (mem_start != NULL) {
        if (munmap(mem_start, MEMORY_SIZE) == -1) {
            perror("munmap");
        }
    }
}


