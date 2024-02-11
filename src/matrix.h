#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Handy macros */
#define MIN(a, b, T) \
   ({ T _a = (a); \
      T _b = (b); \
     _a < _b ? _a : _b; })
     
#define MAX(a, b, T) \
   ({ T _a = (a); \
      T _b = (b); \
     _a > _b ? _a : _b; })

#define SWAP(a, b, T) \
    do {           \
        T tmp = a; \
        a = b; \
        b = tmp; \
    } while (0)

/* Macros for input and output 
 * Improve performance by assuming output matrices do not overlap with
 * input matrices. If this is C++, use the __restrict extension instead
 */


#define TYPEDEF_IN(T) typedef const T* const IN_T
#ifdef __cplusplus
#define restrict __restrict
#endif

typedef uint32_t IDX_t;
typedef double MAT_t;

typedef const IDX_t* const IDX_IN_t;
typedef const MAT_t* const MAT_IN_t;

typedef IDX_t* const restrict IDX_OUT_t;
typedef MAT_t* const restrict MAT_OUT_t;

/* Macros for matrix indexing */
#define MAT_IDX(mat, row, col, n) (mat[(row) * (n) + (col)])

/* Function to print a matrix
 * Inputs:
 *   mat: matrix to print
 *   n: number of rows
 *   m: number of columns of mat
 *   name: name of matrix
 * Example:
 *   MAT_t A[9] = {2, 1, 1, 1, 2, 1, 1, 1, 2};
 *   print_matrix(A, 3, 3, "A"); // prints "A = [2 1 1; 1 2 1; 1 1 2]"
 */
static inline void print_matrix(MAT_IN_t mat, const IDX_t n, const IDX_t m,
                                const char* name) {
    IDX_t i, j;
    printf("%s = [\n", name);
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (j > 0) printf(" ");
            printf("%7.4f", MAT_IDX(mat, i, j, n));
        }
        if (i < n - 1) printf(";\n");
    }
    printf("]\n");
}

/* Function to perform matrix multiplication
 *   C = A * B
 * Inputs:
 *   A: input matrix (m x p)
 *   B: input matrix (p x n)
 *   m: number of rows of A
 *   p: number of columns of A and number of rows of B
 *   n: number of columns of B
 * Output:
 *   C: output matrix (m x n)
 * Example:
 *   MAT_t A[6] = {1, 2, 3, 4, 5, 6};
 *   MAT_t B[6] = {1, 4, 2, 5, 3, 6};
 *   MAT_t C[4];
 *   matrix_multiply(A, B, 2, 3, 2, C); // C = [14 32; 32 77]
 */
static inline void matrix_multiply(MAT_IN_t A, MAT_IN_t B, const IDX_t m, const IDX_t p,
                                   const IDX_t n, MAT_OUT_t C) {
    for (IDX_t i = 0; i < m; i++) {
        for (IDX_t j = 0; j < n; j++) {
            MAT_t sum = 0;
            for (IDX_t k = 0; k < p; k++) {
                sum += MAT_IDX(A, i, k, p) * MAT_IDX(B, k, j, n);
            }
            MAT_IDX(C, i, j, n) = sum;
        }
    }
}
