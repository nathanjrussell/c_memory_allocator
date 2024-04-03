#include "matrix_operations.h"
#include "../memory_allocator/memory_allocator.h"
#include <stdio.h>

#define MALLOC(size) mymalloc(size, __FILE__, __LINE__)

#define FREE myfree


static int** create_identity_matrix(int size);
static int** multiply_matrices(int** matrix1, int** matrix2, int size);
static int isIdentityMatrix(int** matrix, int size);

// Build a test case for profiling and testing using matrix multiplication
MatrixProduct* buildMatrixTest(int matrixSize) {
    MatrixProduct* matrixProduct = (MatrixProduct*)MALLOC(sizeof(MatrixProduct)); //allocate memory for the test structure
    if (matrixProduct == NULL) { //check if memory allocation was successful
        return NULL;
    }
    matrixProduct->matrixSize = matrixSize; //set the size of the matrices
    matrixProduct->matrixA = create_identity_matrix(matrixSize); //create an identity matrix for matrix A
    if (matrixProduct->matrixA == NULL) { //check if memory allocation was successful
        FREE(matrixProduct); //free the allocated memory if problem
        return NULL; //return NULL if problem
    }

    matrixProduct->matrixB = create_identity_matrix(matrixSize);    
    if (matrixProduct->matrixB == NULL) { //check if memory allocation was successful
        // If allocation fails, free previously allocated memory for matrix A
        for (int i = 0; i < matrixSize; i++) {
            FREE(matrixProduct->matrixA[i]); //free the memory for each row
        }
        FREE(matrixProduct); //free the memory for the structure
        return NULL; //return NULL if problem
    } 

    matrixProduct->matrixProductAB = multiply_matrices(matrixProduct->matrixA, matrixProduct->matrixB, matrixSize); //multiply the two matrices
    if (matrixProduct->matrixProductAB == NULL) { //check if memory allocation was successful
        // If allocation fails, free previously allocated memory for matrix A and B
        for (int i = 0; i < matrixSize; i++) {
            FREE(matrixProduct->matrixA[i]); //free the memory for each row of matrix A
            FREE(matrixProduct->matrixB[i]); //free the memory for each row of matrix B
        }
        FREE(matrixProduct->matrixA); //free the memory for matrix A
        FREE(matrixProduct->matrixB); //free the memory for matrix B
        FREE(matrixProduct); //free the memory for the structure
        return NULL; //return NULL if problem
    }
    return matrixProduct; //return the test structure
} //close buildMatrixTest function

// Check if a matrix is an identity matrix (diagonal elements are 1, all other elements are 0)
static int isIdentityMatrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j && matrix[i][j] != 1) {
                return 0; //return 0 if the diagonal element is not 1
            } else if (i != j && matrix[i][j] != 0) {
                return 0; //return 0 if the non-diagonal element is not 0
            }
        }
    }
    return 1; //return 1 if the matrix is an identity matrix
} //close isIdentityMatrix function

// Create an identity matrix of a given size
static int** create_identity_matrix(int size) {
    int** matrix = (int**)MALLOC(size * sizeof(int*)); //allocate memory for the rows
    if (matrix == NULL) { 
        return NULL; //return NULL if memory allocation fails
    }

    for (int i = 0; i < size; i++) { //loop through the rows
        matrix[i] = (int*)MALLOC(size * sizeof(int)); //allocate memory for the entries in the row
        if (matrix[i] == NULL) { //check if memory allocation was successful
            for (int j = 0; j < i; j++) { //loop through the rows and free previously allocated memory
                FREE(matrix[j]); //free the memory for each row
            }
            FREE(matrix); //free the memory for the matrix
            return NULL; //return NULL if problem
        }

        for (int j = 0; j < size; j++) {
            matrix[i][j] = 0; //initialize the elements to 0
        }

        matrix[i][i] = 1; //set the diagonal elements to 1
    } //close for loop
    return matrix; //return the identity matrix
} //close create_identity_matrix function


// Multiply two matrices
static int** multiply_matrices(int** matrix1, int** matrix2, int size) {
    int** result = (int**)MALLOC(size * sizeof(int*)); //allocate memory for the result matrix
    if (result == NULL) { //check if memory allocation was successful
        return NULL; //return NULL if problem
    }

    for (int i = 0; i < size; i++) { //loop through the rows
        result[i] = (int*)MALLOC(size * sizeof(int)); //allocate memory for the entries in the row
        if (result[i] == NULL) { //check if memory allocation was successful
            for (int j = 0; j < i; j++) { //loop through the rows and free previously allocated memory
                FREE(result[j]); //free the memory for each row
            }
            FREE(result); //free the memory for the matrix
            return NULL; //return NULL if problem
        } //close if statement

        for (int j = 0; j < size; j++) { //loop through the entries in the row
            result[i][j] = 0; //initialize the elements to 0
        }
    } //close for loop

    // for loop operations to multiply the matrices
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result; //return the result matrix
} //close multiply_matrices function

// Verify the success of the matrix multiplication
void verifySuccess(MatrixProduct* matrixProduct) {
    if (matrixProduct == NULL) { //check if the matrix product structure is NULL
        matrixProduct->success = 0; //set the success flag to 0
        return; //return if the structure is NULL
    }

    matrixProduct->success  =   isIdentityMatrix(matrixProduct->matrixA, matrixProduct->matrixSize) && //check if matrix A is an identity matrix
                                isIdentityMatrix(matrixProduct->matrixB, matrixProduct->matrixSize) && //check if matrix B is an identity matrix
                                isIdentityMatrix(matrixProduct->matrixProductAB, matrixProduct->matrixSize);  //check if the product matrix is an identity matrix
} //close verifySuccess function

// Destroy the matrix product structure
void destroyMatrixProduct(MatrixProduct* matrixProduct) {
    if (matrixProduct == NULL) { //check if the matrix product structure is NULL
        return; //return if the structure is NULL
    }
    for (int i = 0; i < matrixProduct->matrixSize; i++) { //loop through the rows of the matrices
        FREE(matrixProduct->matrixA[i]); //free the memory for each row of matrix A
        FREE(matrixProduct->matrixB[i]); //free the memory for each row of matrix B
        FREE(matrixProduct->matrixProductAB[i]); //free the memory for each row of the product matrix
    }
    FREE(matrixProduct->matrixA); //free the memory for matrix A
    FREE(matrixProduct->matrixB); //free the memory for matrix B 
    FREE(matrixProduct->matrixProductAB); //free the memory for the product matrix
    FREE(matrixProduct); //free the memory for the structure
} //close destroyMatrixProduct function