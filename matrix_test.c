#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Include string.h for strcpy
#include "matrix_operations/matrix_operations.h"
#include "memory_allocator/memory_allocator.h"
#include <sys/mman.h>


#define MALLOC(size) mymalloc(size, __FILE__, __LINE__)
#define MY_ALLOC(T) (T*)memset(mymalloc(sizeof(T), __FILE__, __LINE__), 0, sizeof(T))
#define FREE myfree

int main() {

    size_t memory_pool_size = 10000*10000; //just large enough for test with seed 123
    initialize_memory_pool(memory_pool_size); 

    int seed = 123;
    srand(seed);
    int testCount = 10000;
    // int testCount = 1;
    int modValue = 300;
    int offsetValue = 50;
    int *randomSizes = (int*)MALLOC(testCount * sizeof(int));
    MatrixProduct** matrixProducts = (MatrixProduct**)MALLOC(testCount * sizeof(MatrixProduct*));
    if (randomSizes == NULL) {
        return 1;
    }
    for (int i = 0; i < testCount; i++) {
        randomSizes[i] = rand() % modValue + offsetValue;
    }
    for (int i=0; i<testCount; i++) {
        int size = randomSizes[i];
        // int size = 4000;
        matrixProducts[i] = buildMatrixTest(size);
        verifySuccess(matrixProducts[i]);
        if (matrixProducts[i] != NULL && matrixProducts[i]->success) {
            printf("Test successful for size %d\n", size);
        } else {
            printf("Test Failed for size %d\n", size);
        }
        if (i >= 2) { // This offsets/delays the deallocation to add more fragmentation
            destroyMatrixProduct(matrixProducts[i-2]);
        }
    }

    if (testCount < 2) { // This is to ensure deallocation for small number of test cases
        for (int i = 0; i < testCount; i++) {
            destroyMatrixProduct(matrixProducts[i]);
        }
    }
    

    FREE(randomSizes);
    FREE(matrixProducts);


    destroy_memory_pool();
    

    return 0;
}