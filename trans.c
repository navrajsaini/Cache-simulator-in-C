/*
 * by: Navraj Saini (Sair4210)
 * 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
   int temp;// diag;
   int blocksz; // 8 for 32x32, 4 64x64, 9 for 61 x 67
   if (N == 32)// for a 32x32 matrix
   {
      blocksz = 8;
   }
   else if (N == 64)
      blocksz = 4;
   else// 61x67 case
   {
      blocksz = 12;
      for (int i = 0; i < M; i+= blocksz)//column
      {
	 for (int j = 0; j < N; j+= blocksz)//row
	 {
	    for (int k = j; (k < j + blocksz) && (k < N); k++)//go through each sub block
	    {
	       for (int l = i; (l < i + blocksz) && (l < M); l++)// since M != N could lead to a an error with the remaining matrix
		                                                 // so added in k < N and l < M
	       {
		  if (k != l)
		     B[l][k] = A[k][l];// the rest of the matrix
		  else
		  {
		     temp = A[k][l];
		  }
	       }// end of most inner for loop
	       if (i == j)
		  B[k][k] = temp;// diagnal matrix
	    }// end of third for loop
	 }// end of second for loop
      }// end of first for loop
   }
   if (N == 32 || N == 64)
   {
      // for 64 and 32
      for (int i = 0; i < N; i+= blocksz)//row
      {
	 for (int j = 0; j < N; j+= blocksz)//column
	 {
	    for (int k = j; k < (j + blocksz); k++)//go through each block
	    {
	       for (int l = i; l < (i + blocksz); l++)
	       {
		  if (k != l)
		     B[l][k] = A[k][l];// the rest of the matrix
		  else
		  {
		     temp = A[k][l];
		  }
	       }// end of most inner for loop
	       if (i == j)
		  B[k][k] = temp;
	    }// end of third for loop
	    
	 }// end of second for loop
      }// end of first for loop
   }
}
/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
       for (j = 0; j < M; j++)
       {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

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
    registerTransFunction(trans, trans_desc); 

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

