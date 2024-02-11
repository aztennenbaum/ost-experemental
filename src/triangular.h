#include "matrix.h"
#include <math.h>

typedef MAT_IN_t TRI_IN_t;
typedef MAT_OUT_t TRI_OUT_t;

/* Macros for triangular indexing 
 * Works for both upper and lower triangular matrices*/
#define TRI_IDX(tri, row, col)                              \
    (tri[((row) > (col) ? ((row) * ((row) + 1)) / 2 + (col) \
                        : ((col) * ((col) + 1)) / 2 + (row))])

/* Function to print a lower triangular matrix
 * Inputs:
 *   L: matrix to print
 *   n: number of rows and columns of mat
 *   name: name of matrix
 * Example:
 *   MAT_t L[6] = {1, 2, 3, 4, 5, 6};
 *   print_lower_triangular_matrix(L, 3, "L"); // prints "L = [1 0 0; 2 3 0; 4 5 6]"
 */
static inline void print_lower_triangular_matrix(TRI_IN_t L, const IDX_t n,
                                                 const char* name) {
    printf("%s = [\n", name);
    for (IDX_t i = 0; i < n; i++) {
        for (IDX_t j = 0; j < n; j++) {
            if (j > 0) printf(" ");
            printf("%7.4f", j > i ? 0 : TRI_IDX(L, i, j));
        }
        if (i < n - 1) printf(";\n");
    }
    printf("]\n");
}

/* Function to print a upper triangular matrix
 * Inputs:
 *   U: matrix to print
 *   n: number of rows and columns of mat
 *   name: name of matrix
 * Example:
 *   MAT_t U[6] = {1, 2, 3, 4, 5, 6};
 *   print_upper_triangular_matrix(U, 3, "U"); // prints "U = [1 2 3; 0 4 5; 0 0 6]"
 */
static inline void print_upper_triangular_matrix(TRI_IN_t U, const IDX_t n,
                                                 const char* name) {
    printf("%s = [\n", name);
    for (IDX_t i = 0; i < n; i++) {
        for (IDX_t j = 0; j < n; j++) {
            if (j > 0) printf(" ");
            printf("%7.4f", j < i ? 0 : TRI_IDX(U, i, j));
        }
        if (i < n - 1) printf(";\n");
    }
    printf("]\n");
}

/* Function to copy a triangular matrix
 * Inputs:
 *   A: input triangular matrix
 *   n: number of rows and columns of A
 * Output:
 *   B: output triangular matrix
 * Example:
 *  MAT_t A[6] = {1, 2, 3, 4, 5, 6};// lower triangular matrix
 *  MAT_t B[6];
 *  copy_triangular_matrix(A, 3, B); // B = [1 0 0; 2 3 0; 4 5 6]
 * Note:
 *   If a lower triangular matrix is copied to an upper triangular matrix,
 *   the output will be the transpose of the input.
 *   The same is true if an upper triangular matrix is copied to a lower
 *   triangular matrix.
 */

static inline void copy_triangular_matrix(TRI_IN_t A, const IDX_t n, TRI_OUT_t B) {
    memcpy(B, A, n * (n + 1) / 2 * sizeof(MAT_t));
}

/* Function to multiply a lower triangular matrix by a upper triangular matrix
 *   A = L * U
 * Inputs:
 *   L: input lower triangular matrix (n x n)
 *   U: input upper triangular matrix (n x n)
 *   n: number of rows and columns of L and U
 * Output:
 *   A: output matrix (n x n)
 *   A = L * U
 * Example:
 *  MAT_t L[3] = {1, 2, 3};
 *  MAT_t U[3] = {1, 2, 3};
 *  MAT_t A[4];
 * LU_multiply(L, U, 2, A); // A = [1 2; 2 13]
 */
static inline void LU_multiply(TRI_IN_t L, TRI_IN_t U, const IDX_t n, MAT_OUT_t A) {
    for (IDX_t i = 0; i < n; i++) {
        for (IDX_t j = 0; j < n; j++) {
            MAT_IDX(A, i, j, n) = 0;
            IDX_t min             = i < j ? i : j;
            for (IDX_t k = 0; k <= min; k++) {
                MAT_IDX(A, i, j, n) += TRI_IDX(L, i, k) * TRI_IDX(U, k, j);
            }
        }
    }
}

/* Function to perform cholesky decomposition of matrix
 *   A = L * L^T
 * Inputs:
 *   A: input matrix (positive definite)
 *   n: number of rows and columns of A
 * Output:
 *   L: lower triangular matrix such that A = L * L^T
 * Example:
 *   MAT_t A[9] = {2, 1, 1, 1, 2, 1, 1, 1, 2};
 *   MAT_t L[6];
 *   cholesky(A, 3, L); // L = [1.41421 0 0; 0.707107 0.707107 0; 1.22474 0.816497 1.1547]
 */
static inline void cholesky(MAT_IN_t A, const IDX_t n, TRI_OUT_t L) {
    for (IDX_t i = 0; i < n; i++) {
        for (IDX_t j = 0; j <= i; j++) {
            MAT_t sum = 0;
            for (IDX_t k = 0; k < j; k++) {
                sum += TRI_IDX(L, i, k) * TRI_IDX(L, j, k);
            }
            if (i == j) {
                TRI_IDX(L, i, j) = sqrt(MAT_IDX(A, i, i, n) - sum);
            } else {
                TRI_IDX(L, i, j) = 1 / TRI_IDX(L, j, j) * (MAT_IDX(A, i, j, n) - sum);
            }
        }
    }
}

/* Function to divide a vector by a lower triangular matrix
 *   y = L^-1 * x
 * Inputs:
 *   L: lower triangular matrix (n x n)
 *   x: input vector (n x 1)
 *   n: number of rows and columns of L and length of x
 * Output:
 *   y: output vector (n x 1)
 * Example:
 *   MAT_t L[6] = {1, 2, 3, 4, 5, 6};
 *   MAT_t x[3] = {1, 2, 3};
 *   MAT_t y[3];
 *   L_divide(L, x, 2, y); // y = [-1 1.5 -0.5]
 */
static inline void L_divide(TRI_IN_t L, MAT_IN_t x, const IDX_t n, MAT_OUT_t y) {
    for (IDX_t i = 0; i < n; i++) {
        MAT_t sum = 0;
        for (IDX_t j = 0; j < i; j++) {
            sum += TRI_IDX(L, i, j) * y[j];
        }
        y[i] = (x[i] - sum) / TRI_IDX(L, i, i);
    }
}
