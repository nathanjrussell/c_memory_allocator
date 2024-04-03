#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

// MatrixProduct structure to keep track of the matrices and the result of the product for testing
typedef struct {
    int matrixSize;
    int** matrixA;
    int** matrixB;
    int** matrixProductAB;
    char success; // 1 if the test was successful, 0 otherwise
} MatrixProduct;

MatrixProduct* buildMatrixTest(int matrixSize); // Build simple matrices for testing
void verifySuccess(MatrixProduct* matrixProduct); // Verify if the product of the matrices is correct 
void destroyMatrixProduct(MatrixProduct* matrixProduct); // Free the memory allocated for the matrices

#endif // MATRIX_OPERATIONS_H