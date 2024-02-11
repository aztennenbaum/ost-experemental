#include "kdtree.h"
#include "test.h"

//function to insertion sort results[] based on the values if idx[result[i]]
static inline void insertion_sort(IDX_OUT_t results, IDX_IN_t idx, IDX_t n) {
    for (IDX_t i = 1; i < n; i++) {
        IDX_t j = i;
        while (j > 0 && idx[results[j]] < idx[results[j-1]]) {
            SWAP(results[j], results[j-1], IDX_t);
            j--;
        }
    }
}

//function to quicksort results[] based on the values if idx[result[i]]
//using insertion sort for small arrays less than threshold
static inline void quicksort(IDX_OUT_t results, IDX_IN_t idx, const IDX_t n, const IDX_t threshold) {
    //assert(n>0);
    if (n > threshold) {
        IDX_t pivot = idx[results[n/2]];
        IDX_t i = 0;
        IDX_t j = n - 1;
        while (1) {
            while (idx[results[i]] < pivot) {
                i++;
            }
            while (idx[results[j]] > pivot) {
                j--;
            }
            if (i >= j) {
                break;
            }
            SWAP(results[i], results[j], IDX_t);
            i++;
            j--;
        }
        quicksort(results, idx, i, threshold);
        quicksort(results + i, idx, n - i, threshold);
    } else {
        insertion_sort(results, idx, n);
    }
}


#define ROWS 500000
MAT_t points_xy[ROWS*2], range[2*2];
IDX_t idx[ROWS], results[ROWS];

#define THRESHOLD 64
int main() {
    IDX_t n = (IDX_t) MAX(in(),0, int);
    for (IDX_t i = 0; i < n; i++) {
        points_xy[i*2]=(MAT_t)in();
        points_xy[i*2+1]=(MAT_t)in();
        idx[i] = (IDX_t)i;
    }
    kdbuild(points_xy, n, 2, 0, n, 0, THRESHOLD, idx);
    IDX_t q = (IDX_t) MAX(in(),0, int);
    for (IDX_t i = 0; i < q; i++) {
        range[0]=in();
        range[2]=in();
        range[1]=in();
        range[3]=in();
        IDX_t count = kdsearch(points_xy, n, 2, 0, THRESHOLD, range, 0, n, results);
        //insertion_sort(results, idx, count);
        quicksort(results, idx, count, THRESHOLD);
        for (IDX_t j = 0; j < count; j++) {
            out((int)idx[results[j]]);
            pc(10);
        }
        // put newline
        pc(10);
    }
    return 0;
}
