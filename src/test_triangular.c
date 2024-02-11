#include "triangular.h"

int main() {
    double A[9] = { 2, 1, 1, 1, 2, 1, 1, 1, 2 };
    double L[6] = { 1, 2, 3, 4, 5, 6 }; // L = [1 0 0; 2 3 0; 4 5 6]
    double U[6] = { 1, 2, 3, 4, 5, 6 }; // U = L' = [1 2 4; 0 3 5; 0 0 6]

    // print lower triangular matrix L
    print_lower_triangular_matrix(L, 3, "L");

    // print upper triangular matrix U
    print_upper_triangular_matrix(U, 3, "U");

    cholesky(A, 3, L);
    print_matrix(A, 3, 3, "A");
    print_lower_triangular_matrix(L, 3, "L");

    // copy L to U
    copy_triangular_matrix(L, 3, U);
    print_upper_triangular_matrix(U, 3, "U");

    // L * U
    double LU[9];
    LU_multiply(L, U, 3, LU);
    print_matrix(LU, 3, 3, "LU");

    double x[3] = { 1, 2, 3 };
    double y[3];
    L_divide(L, x, 3, y);

    return 0;
}
