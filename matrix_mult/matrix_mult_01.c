#include <stdio.h>

#include <time.h>

#define MAX_SIZE 1000

int main()

{

    // matrix sizes

    const int m = MAX_SIZE;

    const int n = MAX_SIZE;

    const int p = MAX_SIZE;

    // matrix

    static double A[MAX_SIZE][MAX_SIZE];

    static double B[MAX_SIZE][MAX_SIZE];

    static double C[MAX_SIZE][MAX_SIZE];

    // init to 1.1

    for (int i = 0; i < m; i++)

    {

        for (int j = 0; j < n; j++)

        {

            A[i][j] = 1.1;
            B[i][j] = 1.1;
            C[i][j] = 0.0;
        }
    }

    // start timer

    clock_t start = clock();

    // mult

    for (int i = 0; i < m; i++)

    {

        for (int j = 0; j < p; j++)

        {

            double f = 0;

            for (int k = 0; k < n; k++)

            {

                f += A[i][k] * B[k][j];
            }

            C[i][j] = f;
        }
    }

    clock_t end = clock();

    // for (int i = 0; i < m; i++)

    // {

    // for (int j = 0; j < p; j++)

    // {

    //  printf("%.2f", C[i][j]);

    //  printf(" ");

    // }

    // printf("\n");

    // }

    double exeTime = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Loop i-j-k: %.3f s", exeTime);

    return 0;
}