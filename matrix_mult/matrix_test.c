#include <stdio.h>
#include <time.h>

int main() {

    // matrix sizes

    const int m = 2;
    const int n = 3;
    const int p = 2;

    // matrix

    static double A[2][3] = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
    static double B[3][2] = {{7.0, 8.0}, {9.0, 1.0}, {2.0, 3.0}};
    static double C[2][2];

    // start timer

    clock_t start = clock();

    static double B_T[2][3];

    // transpose
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            B_T[j][i] = B[i][j];

    for (int j = 0; j < p; j++) {

        for (int i = 0; i < m; i++) {

            double f = 0;

            for (int k = 0; k < n; k++) {

                f += A[i][k] * B_T[j][k];
            }

            C[i][j] = f;
        }
    }

    clock_t end = clock();

    for (int i = 0; i < m; i++)
    {

        for (int j = 0; j < p; j++)
        {

            printf("%.2f", C[i][j]);

            printf(" ");
        }

        printf("\n");
    }

    double exeTime = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Loop-reordering: %.3f s", exeTime);

    return 0;
}