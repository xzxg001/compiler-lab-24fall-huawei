#include <math.h>
#include <stdlib.h>
typedef struct {
  int a;
  double b;
  char c;
  float d;
  short e;
  int f;
} str_t1;
typedef struct {
  str_t1 tmp;
  str_t1 *st;
} str_t2;
str_t2 glob_struct;
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
int main() {
  int i, r;
  r = rand();
  num = 300000000;
  str_t1 *p1 = (str_t1 *)calloc(num, sizeof(str_t1));
  glob_struct.st = p1;
  if (glob_struct.st == NULL)
    return 0;
  for (i = 0; i < num; i++)
    glob_struct.st[i].a = 1;
  return 0;
}