#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int** matA;
int** matB;
int** matSumResult;
int** matDiffResult;
int** matProductResult;

typedef struct {
    int row;
    int col;
    int size;
} MatrixCell;

void fillMatrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args) {
    MatrixCell* cell = (MatrixCell*)args;
    matSumResult[cell->row][cell->col] = matA[cell->row][cell->col] + matB[cell->row][cell->col];
    free(cell);
    return NULL;
}

void* computeDiff(void* args) {
    MatrixCell* cell = (MatrixCell*)args;
    matDiffResult[cell->row][cell->col] = matA[cell->row][cell->col] - matB[cell->row][cell->col];
    free(cell);
    return NULL;
}

void* computeProduct(void* args) {
    MatrixCell* cell = (MatrixCell*)args;
    matProductResult[cell->row][cell->col] = 0;
    for (int k = 0; k < cell->size; k++) {
        matProductResult[cell->row][cell->col] += matA[cell->row][k] * matB[k][cell->col];
    }
    free(cell);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        printf("Invalid matrix size. It should be greater than 0\n");
        return 1;
    }

    // Dynamically allocate memory for matrices
    matA = (int**)malloc(N * sizeof(int*));
    matB = (int**)malloc(N * sizeof(int*));
    matSumResult = (int**)malloc(N * sizeof(int*));
    matDiffResult = (int**)malloc(N * sizeof(int*));
    matProductResult = (int**)malloc(N * sizeof(int*));

    for (int i = 0; i < N; i++) {
        matA[i] = (int*)malloc(N * sizeof(int));
        matB[i] = (int*)malloc(N * sizeof(int));
        matSumResult[i] = (int*)malloc(N * sizeof(int));
        matDiffResult[i] = (int*)malloc(N * sizeof(int));
        matProductResult[i] = (int*)malloc(N * sizeof(int));
    }

    srand(time(0));

    // Fill matrices
    fillMatrix(matA, N);
    fillMatrix(matB, N);

    printf("Matrix A:\n");
    printMatrix(matA, N);
    printf("Matrix B:\n");
    printMatrix(matB, N);

    pthread_t sum_threads[N][N];
    pthread_t diff_threads[N][N];
    pthread_t product_threads[N][N];

    // Create threads for sum, diff, and product
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Allocate memory for each thread argument
            MatrixCell* sum_cell = (MatrixCell*)malloc(sizeof(MatrixCell));
            sum_cell->row = i;
            sum_cell->col = j;
            sum_cell->size = N;

            MatrixCell* diff_cell = (MatrixCell*)malloc(sizeof(MatrixCell));
            diff_cell->row = i;
            diff_cell->col = j;
            diff_cell->size = N;

            MatrixCell* product_cell = (MatrixCell*)malloc(sizeof(MatrixCell));
            product_cell->row = i;
            product_cell->col = j;
            product_cell->size = N;

            // Create threads for sum, diff, and product
            pthread_create(&sum_threads[i][j], NULL, computeSum, (void*)sum_cell);
            pthread_create(&diff_threads[i][j], NULL, computeDiff, (void*)diff_cell);
            pthread_create(&product_threads[i][j], NULL, computeProduct, (void*)product_cell);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pthread_join(sum_threads[i][j], NULL);
            pthread_join(diff_threads[i][j], NULL);
            pthread_join(product_threads[i][j], NULL);
        }
    }

    // Print the results
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult, N);
    printf("Difference:\n");
    printMatrix(matDiffResult, N);
    printf("Product:\n");
    printMatrix(matProductResult, N);

    // Free dynamically allocated memory
    for (int i = 0; i < N; i++) {
        free(matA[i]);
        free(matB[i]);
        free(matSumResult[i]);
        free(matDiffResult[i]);
        free(matProductResult[i]);
    }
    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);

    return 0;
}
