// Submitted by Niket Gautam
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "lab3.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char trans1_desc[] = "For input: -M 32 -N 32";
void trans1(int M, int N, int A[N][M], int B[M][N]){
	int i, j, k, m, n, temp;
	for (i = 0; i<M; i=i+4) { 
		for (j = 0; j<M; j=j+4) {
			for (k = i; k<i+4; k++) {
				for (m = j; m<j+4; m++) {
					if (k != m)
						B[m][k] = A[k][m];
					else{
						temp = A[k][m];
						n = k;
					}
				}
				if (i == j)
					B[n][n] = temp;
			}	
		}
	}
}

char trans2_desc[] = "For input: -M 64 -N 64";
void trans2(int M, int N, int A[N][M], int B[M][N]){
	int i, j, k, m, n, temp;
	for (i = 0; i<M; i=i+8) { 
		for (j = 0; j<M; j=j+8) {
			for (k = i; k<i+8; k++) {
				for (m = j; m<j+8; m++) {
					if (k != m)
						B[m][k] = A[k][m];
					else{
						temp = A[k][m];
						n = k;
					}
				}
				if (i == j)
					B[n][n] = temp;
			}	
		}
	}
}

char trans3_desc[] = "For inputs other than -M 32 -N 32 | -M 64 -N 64";
void trans3(int M, int N, int A[N][M], int B[M][N]){
	int i, j, k, m, n, temp;
	for (i = 0; i<M; i=i+16) { 
		for (j = 0; j<M; j=j+16) {
			for (k = i; k<i+16; k++) {
				for (m = j; m<j+16; m++) {
					if (k != m)
						B[m][k] = A[k][m];
					else{
						temp = A[k][m];
						n = k;
					}
				}
				if (i == j)
					B[n][n] = temp;
			}	
		}
	}
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
	if( M==32)
		trans1(M, N, A, B);
	else if( M==64)
		trans2(M, N, A, B);
	else
		trans3(M, N, A, B);
}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    //registerTransFunction(trans1, trans1_desc);
	//registerTransFunction(trans2, trans2_desc);
	//registerTransFunction(trans3, trans3_desc);	

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

