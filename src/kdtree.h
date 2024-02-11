#include "matrix.h"

/* Function to swap rows of a matrix
 * Inputs:
 *   A: input matrix
 *   n: number of columns of A
 *   i: index of the first row to swap
 *   j: index of the second row to swap
 * Output:
 *   A: matrix with rows i and j swapped
 * Example:
 *   MAT_t A[6] = {1, 2, 3, 4, 5, 6};
 *   row_swap(A, 2, 0, 1); // A = [4 5 6; 1 2 3]
 */
static inline void row_swap(MAT_OUT_t A, const IDX_t n, const IDX_t i,
                             const IDX_t j) {
    for (IDX_t k = 0; k < n; k++) {
        SWAP(MAT_IDX(A, i, k, n), MAT_IDX(A, j, k, n),MAT_t);
    }
}

/* Function to quickselect a matrix by rows and output an index vector
 * Inputs:
 *     A: input matrix (m x n)
 *     m: number of rows of A
 *     n: number of columns of A
 *     col: column to sort by
 *     k: index of the element to select
 *     idx: index vector (m x 1) - unused if null
 * Output:
 *     A: matrix with the k-th element of the col-th column in its correct position
 *     idx: index vector with the indices of the k smallest elements
 * Example:
 *     MAT_t A[6] = {3, 2, 1, 6, 5, 4};
 *     IDX_t idx[6] = {0, 1, 2, 3, 4, 5};
 *     kdqselect(A, 2, 3, 0, 1, idx); // A = [1 2 3; 4 5 6], idx = [2 1 0 5 4 3]
 *     print_matrix(A, 2, 3, "A"); // prints "A = [1 2 3; 4 5 6]"
 */

static inline void kdqselect(MAT_OUT_t A, const IDX_t m, const IDX_t n,
                             const IDX_t col, const IDX_t k, IDX_OUT_t idx) {
    //assert(m>0);
    IDX_t l = 0;
    IDX_t r = m - 1;
    while (l < r) {
        double pivot = MAT_IDX(A, (l + r) / 2, col, n);
        IDX_t i = l;
        IDX_t j = r;
        while (i <= j) {
            while (MAT_IDX(A, i, col, n) < pivot) {
                i++;
            }
            while (MAT_IDX(A, j, col, n) > pivot) {
                j--;
            }
            if (i <= j) {
                row_swap(A, n, i, j);
                if (idx != NULL) {
                    SWAP(idx[i], idx[j], IDX_t);
                }
                i++;
                j--;
            }
        }
        if (k <= j) {
            r = j;
        } else if (k >= i) {
            l = i;
        } else {
            break;
        }
    }
}

/* Function to check if a row is in a range
 * Inputs:
 *     A: input matrix (m x n)
 *     n: number of columns of A
 *     range: range to search (2 x n)
 *     row: row to check
 * Output:
 *     1 if the row is in the range, 0 otherwise
 * Example:
 *     MAT_t A[6] = {1, 2, 3, 4, 5, 6};
 *     MAT_t range[4] = {2, 3, 5, 6};
 *     int is_in_range = row_in_range(A, 3, range, 0); // is_in_range = 1
 */
static inline int row_in_range(MAT_IN_t A, const IDX_t n, MAT_IN_t range,
                                  const IDX_t row) {
    for (IDX_t i = 0; i < n; i++) {
        if (MAT_IDX(A, row, i, n) < MAT_IDX(range, 0, i, n)
            || MAT_IDX(A, row, i, n) > MAT_IDX(range, 1, i, n)) {
            return 0;
        }
    }
    return 1;
}

/* Function to search for rows in a range
 * Inputs:
 *   A: input matrix (m x n)
 *   m: number of rows of A
 *   n: number of columns of A
 *   range: range to search (2 x n)
 *   leaf_start: index of the first row in the branch
 *   leaf_size:  number of rows in the branch
 *   depth: depth of the current node in the k-d tree
 *   threshold: threshold to switch to linear search

 * Output:
 *   results: index vector with the indices of the rows in the range
 * Returns:
 *   number of rows in the range
 * Example:
 *  MAT_t A[14] = {2, 1, 2, 2, 4, 2, 6, 2, 3, 3, 5, 4, 2, 4};
 *  MAT_t range[4] = {2, 4, 0, 4};
 *  IDX_t results[4];
 *  IDX_t count = kdsearch(A, 7, 2, range, 0, 7 0, 1, results); // count = 4, results = {0, 1, 2, 3}
 */
static inline IDX_t kdsearch(MAT_IN_t A, const IDX_t m, const IDX_t n, const IDX_t depth,
                               const IDX_t threshold, MAT_IN_t range, const IDX_t leaf_start,
                               const IDX_t leaf_size, IDX_OUT_t results) {
    if (leaf_size > threshold) { // k-d tree search
        IDX_t col = depth % n;
        IDX_t k = leaf_size / 2;
        IDX_t leaf_mid = leaf_start + k;
        IDX_t count = 0;
        if (MAT_IDX(A, leaf_mid, col, n) >= MAT_IDX(range, 0, col, n)) {
            count += kdsearch(A, m, n, depth + 1, threshold, range, leaf_start, k,
                                  results);
        }
        if (row_in_range(A, n, range, leaf_mid)) {
            results[count++] = leaf_mid;
        }
        if (MAT_IDX(A, leaf_mid, col, n) <= MAT_IDX(range, 1, col, n)) {
            count += kdsearch(A, m, n, depth + 1, threshold, range, leaf_start + (k+1),
                                  leaf_size - (k+1), results + count);
        }
        return count;
    } else { // linear search
        IDX_t count = 0;
        for (IDX_t i = leaf_start; i < leaf_start + leaf_size; i++) {
            if (row_in_range(A, n, range, i)) {
                results[count++] = i;
            }
        }
        return count;
    }
}

/* Function to build a k-d tree in place but stop when the number of elements
 * in a node is less than or equal to a threshold
 * Inputs:
 *     A: input matrix (m x n)
 *     m: number of rows of A
 *     n: number of columns of A
 *     leaf_start: index of the first row in the branch
 *     leaf_size:  number of rows in the branch
 *     depth: depth of the current node in the k-d tree
 *     threshold: threshold to stop building the k-d tree
 * Output:
 *     idx: index vector with the indices of the k smallest elements
 */
static inline void kdbuild(MAT_OUT_t A, const IDX_t m, const IDX_t n,
                                const IDX_t leaf_start, const IDX_t leaf_size,
                                const IDX_t depth, const IDX_t threshold, IDX_OUT_t idx) {
    if (leaf_size > threshold) {
        IDX_t col = depth % n;
        IDX_t k = leaf_size / 2;
        kdqselect(A + leaf_start * n, leaf_size, n, col, k, idx + leaf_start);
        kdbuild(A, m, n, leaf_start, k, depth + 1, threshold, idx);
        kdbuild(A, m, n, leaf_start + (k+1), leaf_size - (k+1), depth + 1, threshold, idx);
    }
}
