#include <math.h>
#include <stdlib.h>

typedef struct {
  int *a;
  double *b;
  char *c;
  float *d;
  short *e;
  int *f;
} str_t2_fields;
#ifdef STACK_SIZE
#if STACK_SIZE > 16000
#define N 1000
#else
#define N (STACK_SIZE / 16)
#endif
#else
#define N 1000
#endif
int num;

str_t2_fields glob_struct;

int main() {
  int i, r;
  r = rand();
  num = 300000000;

  // Allocate memory for each field separately
  glob_struct.a = (int *)calloc(num, sizeof(int));
  glob_struct.b = (double *)calloc(num, sizeof(double));
  glob_struct.c = (char *)calloc(num, sizeof(char));
  glob_struct.d = (float *)calloc(num, sizeof(float));
  glob_struct.e = (short *)calloc(num, sizeof(short));
  glob_struct.f = (int *)calloc(num, sizeof(int));

  // Check if any allocation failed
  if (!glob_struct.a || !glob_struct.b || !glob_struct.c || !glob_struct.d ||
      !glob_struct.e || !glob_struct.f) {
    return 0;
  }

  // Initialize the `a` field as before
  for (i = 0; i < num; i++)
    glob_struct.a[i] = 1;

  return 0;
}
