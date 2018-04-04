#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>       /* time */

#define SM (64 / sizeof (double))
// SM is the size of the cache line, divided by the size of the double
// LEVEL1_DCACHE_LINESIZE = 64 (for the system it's been coded on)

int **res;
struct timeval t0, t1;

void mm(int** mul1, int** mul2, int size) {

  gettimeofday(&t0, 0);

  for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
          for (int k = 0; k < size; k++)
              res[i][j] += mul1[i][k] * mul2[k][j];

  gettimeofday(&t1, 0);
}

void mm_transposed(int **mul1, int **mul2, int N) {

  int i, j, k;
  double tmp[N][N];

  gettimeofday(&t0, 0);

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      tmp[i][j] = mul2[j][i];
  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      for (k = 0; k < N; ++k)
        res[i][j] += mul1[i][k] * tmp[j][k];

  gettimeofday(&t1, 0);
}

void mm_cache_optimized(int **mul1, int **mul2, int N) {

  int i, j, k, i2, k2, j2;
  int *rmul1, *rmul2, *rres;

  gettimeofday(&t0, 0);

  for (i = 0; i < N; i += SM)
      for (j = 0; j < N; j += SM)
          for (k = 0; k < N; k += SM)
              for (i2 = 0, rres = &res[i][j], rmul1 = &mul1[i][k]; i2 < SM; ++i2, rres += N, rmul1 += N)
                  for (k2 = 0, rmul2 = &mul2[k][j]; k2 < SM; ++k2, rmul2 += N)
                      for (j2 = 0; j2 < SM; ++j2)
                          rres[j2] += rmul1[k2] * rmul2[j2];

  gettimeofday(&t1, 0);
}

void print_matrix(int **matrix, int matrix_size) {
  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++)
      printf("%d  ", matrix[i][j]);

    printf("\n");
  }
}

void generate_random_matrix(int **matrix, int size) {
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      matrix[i][j] = arc4random()%9;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "usage: size, argc: %d\n", argc);
    exit(1);
  }

  if (argv[1] < 0) {
    fprintf(stderr, "invalid values provided");
    exit(1);
  }

  int array_size = atoi(argv[1]);
  // Allocs and inits matrices we'll be working on
  int **mul1 = malloc(array_size * sizeof * mul1);
  int **mul2 = malloc(array_size * sizeof * mul2);
  res        = malloc(array_size * sizeof * res);

  for (int i = 0; i < array_size; i++)
    mul1[i] = malloc(array_size * sizeof *mul1[i]);

  for (int j = 0; j < array_size; j++)
    mul2[j] = malloc(array_size * sizeof *mul2[j]);

  for (int k = 0; k < array_size; k++)
    res[k]  = malloc(array_size * sizeof *res[k]);

  // Initializes global variable
  // holding the timestamp of the start of the calculations

  // Generates random values for the matrices
  generate_random_matrix(mul1, array_size);
  generate_random_matrix(mul2, array_size);

  //print_matrix(mul1, array_size);
  //printf("\n");
  //print_matrix(mul2, array_size);

  printf("Calculating matrices...\n");

  // Multiplies the matrices using the "standard" algorithm.
  mm(mul1, mul2, array_size);

  long elapsed = ((t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec)/1000;
  printf("Normal time: %ld miliseconds\n", elapsed);

  // Multiplies the matrices using transpositions.
  mm_transposed(mul1, mul2, array_size);

  elapsed = ((t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec)/1000;
  printf("Transposed time: %ld miliseconds\n", elapsed);

  // Multiplies the matrices using the algorithm which is best suited
  // towards modern CPU cache capabilities
  mm_cache_optimized(mul1, mul2, array_size);

  elapsed = ((t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec)/1000;
  printf("Cache optimized time: %ld miliseconds\n", elapsed);

  mm_cache_optimized(mul1, mul2, array_size);

  //print_matrix(res, array_size);
}
