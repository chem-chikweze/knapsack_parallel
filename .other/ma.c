#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define nrow 2500                /* number of rows in matrix */
#define ncol 2500                /* number of columns in matrix */

int main (int argc, char *argv[]) {

   int tid;
   int nthreads;
   int i;
   int j;
   int k;
   int chunk;

   double a[nrow][ncol];
   double b[nrow][ncol];
   double c[nrow][ncol];
   double sum;

   chunk = 10;                    /* set loop iteration chunk size */

   double start_time;
   double end_time;

   char buffer[256];

   start_time = omp_get_wtime();

   gethostname(buffer, sizeof(buffer));
   printf("Running benchmark on: %s\n", buffer);

   /* parallel region */
   #pragma omp parallel shared(a, b, c, nthreads, chunk) private(tid, i, j, k)
   {
      tid = omp_get_thread_num();

      if (tid == 0) {
         nthreads = omp_get_num_threads();
         printf("Starting with %d threads\n", nthreads);
         printf("Initializing matrices\n");
      }

      /* Initialize matrices */
      #pragma omp for schedule (static, chunk) 
      for (i = 0; i < nrow; i++) {
         for (j = 0; j < ncol; j++) {
            a[i][j]= i + j;
         }
      }
      #pragma omp for schedule (static, chunk)
      for (i = 0; i < ncol; i++) {
         for (j = 0; j < ncol; j++) {
            b[i][j]= i * j;
         }
      }
      #pragma omp for schedule (static, chunk)
      for (i = 0; i < nrow; i++) {
         for (j = 0; j < ncol; j++) {
            c[i][j]= 0;
         }
      }

   /* Matrix multiply sharing iterations on outer loop */
      printf("Thread %d starting matrix multiplication\n", tid);

      #pragma omp for schedule (static, chunk)
      for (i = 0; i < nrow; i++) {
         /* printf("thread %d has completed row %d\n", tid, i); */
         for(j = 0; j < ncol; j++) {
            for (k = 0; k < ncol; k++) {
               c[i][j] += a[i][k] * b[k][j];
            }
         }
      }
   }   /* End of parallel region */

   printf("Matrix multiplication is done. End of parallel task.\n");
   /* print sum of columns */
   for (i = 0; i < ncol; i++) {
      sum = 0.0;
      for (j = 0; j < nrow; j++) {
         sum += c[i][j];
      }
      /* printf("Sum of column %d in product matrix is %lf\n", i, sum); */
   }

   printf("Matrix column sum is done. End of serial task.\n");

   end_time = omp_get_wtime();

   printf("Wall time was %lf seconds.\n", end_time - start_time);

   return(0);

}