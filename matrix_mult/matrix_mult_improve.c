#include <stdio.h>
#include <time.h>
 
#define MAX_SIZE 2000
#define SUB_SIZE 200
 
// matrix sizes
const int m = MAX_SIZE;
const int n = MAX_SIZE;
const int p = MAX_SIZE;
 
// matrix
static double A[MAX_SIZE * MAX_SIZE];
static double B[MAX_SIZE * MAX_SIZE];
static double C[MAX_SIZE * MAX_SIZE];
 
// functions
void init_matrix() {
 
    for (int i = 0; i < m; i++) {
 
        for (int j = 0; j < n; j++) {
 
            A[i * m + j] = 1.1;
            B[i * m + j] = 1.1;
            C[i * m + j] = 0.0;
        }
    }
}
 
void reset_c() {
 
    for (int i = 0; i < m; i++) {
 
        for (int j = 0; j < p; j++) {
 
            C[i * m + j] = 0.0;
        }
    }
}
 
void print_matrix() {
 
    for (int i = 0; i < m; i++) {
 
        for (int j = 0; j < p; j++) {
 
            printf("%.2f", C[i * m + j]);
            printf(" ");
 
        }
 
        printf("\n");
    }
}
 
void show_execution_time(const char *label, clock_t start, clock_t end) {
 
    double exeTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("%-16s: %6.3f s\n", label, exeTime);
}
 
void loop_ijk() {
 
    // reset
    reset_c();
 
    // start timer
    clock_t start = clock();
 
    for (int i = 0; i < m; i++) {
 
        for (int j = 0; j < p; j++) {
 
            double f = 0;
 
            for (int k = 0; k < n; k++) {
 
                f += A[i * m + k] * B[k * n + j];
            }
 
            C[i * m + j] = f;
        }
    }
 
    clock_t end = clock();
    show_execution_time("Loop i-j-k", start, end);
}
 
void loop_reordering() {
 
    // reset
    reset_c();
 
    // start timer
    clock_t start = clock();
 
    for (int i = 0; i < m; i++) {
 
        for (int k = 0; k < n; k++) {
 
            double f = A[i * m + k];
 
            for (int j = 0; j < p; j++) {
 
                C[i * m + j] += f * B[k * n + j];
            }
        }
    }
 
    clock_t end = clock();
    show_execution_time("Loop reordering", start, end);
}
 
void matrix_column() {
 
    // reset
    reset_c();
 
    // start timer
    clock_t start = clock();
 
    static double sub_col_j[MAX_SIZE];
    static double temp[MAX_SIZE];
 
    // cut B matrix into column vectors
    for (int j = 0; j < p; j++) {
 
        // get column
        for (int i = 0; i < n; i++) {
 
            sub_col_j[i] = B[i * n + j];
        }
 
        // mult
        for (int i = 0; i < m; i++) {
 
            double f = 0;
 
            for (int k = 0; k < n; k++) {
 
                f += A[i * m + k] * sub_col_j[k];
            }
            
            temp[i] = f;
        }

        for (int i = 0; i < m; i++) {
            C[i * m + j] = temp[i];
        }
    }
 
    clock_t end = clock();
    show_execution_time("Matrix-column", start, end);
}
 
void block_without_copy() {
 
    // reset
    reset_c();
 
    // start timer
    clock_t start = clock();
 
    static double sub_col_j[MAX_SIZE];
 
    // mult
    for (int ii = 0; ii < m; ii += SUB_SIZE) {
 
        for (int kk = 0; kk < n; kk += SUB_SIZE) {
 
            for (int jj = 0; jj < p; jj += SUB_SIZE) {
 
                // use matrix-column
                for (int j = jj; j < jj + SUB_SIZE; j++) {
 
                    // get column
                    for (int i = 0; i < SUB_SIZE; i++) {
 
                        sub_col_j[i] = B[(kk + i) * SUB_SIZE + j];
                    }
 
                    // mult
                    for (int i = ii; i < ii + SUB_SIZE; i++) {
 
                        double f = 0;
 
                        for (int k = 0; k < SUB_SIZE; k++) {
 
                            f += A[i * (ii + SUB_SIZE) + kk + k] * sub_col_j[k];
                        }
 
                        C[i * (ii + SUB_SIZE) + j] += f;
                    }
                }
            }
        }
    }
 
    clock_t end = clock();
    show_execution_time("Block w/o copy", start, end);
}
 
void block_with_copy() {
 
    // reset
    reset_c();
 
    // start timer
    clock_t start = clock();
 
    static double sub_col_j[MAX_SIZE];
 
    static double sub_A[SUB_SIZE * SUB_SIZE];
    static double sub_B[SUB_SIZE * SUB_SIZE];
 
    // mult
    for (int ii = 0; ii < m; ii += SUB_SIZE) {
 
        for (int kk = 0; kk < n; kk += SUB_SIZE) {
 
            // copy A block
            for (int i = 0; i < SUB_SIZE; i++) {
 
                for (int k = 0; k < SUB_SIZE; k++) {
 
                    sub_A[i * SUB_SIZE + k] = A[(ii + i) * SUB_SIZE + kk + k];
                }
            }
 
            for (int jj = 0; jj < p; jj += SUB_SIZE) {
 
                // copy B block
                for (int k = 0; k < SUB_SIZE; k++) {
 
                    for (int j = 0; j < SUB_SIZE; j++) {
 
                        sub_B[k * SUB_SIZE + j] = B[(kk + k) * SUB_SIZE + jj + j];
                    }
                }
 
                // use matrix-column
                for (int j = jj; j < jj + SUB_SIZE; j++) {
 
                    // get column from sub_B
                    for (int k = 0; k < SUB_SIZE; k++)
                        sub_col_j[k] = sub_B[k * SUB_SIZE + j - jj];
 
                    // mult
                    for (int i = ii; i < ii + SUB_SIZE; i++) {
                        double f = 0;
                        for (int k = 0; k < SUB_SIZE; k++)
                            f += sub_A[(i - ii) * SUB_SIZE + k] * sub_col_j[k];
                        C[i * (ii + SUB_SIZE) + j] += f;
                    }
                }
            }
        }
    }
 
    clock_t end = clock();
    show_execution_time("Block with copy", start, end);
}
 
int main() {
 
    // init
    init_matrix();
 
    // run mult
    loop_ijk();
    loop_reordering();
    matrix_column();
    block_without_copy();
    block_with_copy();
 
    return 0;
}