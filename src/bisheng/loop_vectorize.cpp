// from USTC bisheng compiler lab
// http://staff.ustc.edu.cn/~yuzhang/compiler/lectures/bishengLab.pdf
#include <cstdio>
#include <math.h>
#include <time.h>
void __attribute__((noinline)) f(int N, float *arr1, float *arr2,
                                 float *result) {
  for (int i = 0; i < N; i++) {
    result[i] = arr1[i] + arr2[i];
  }
}//样例用来进行数组相加，存到新数组中
int main() {
  clock_t start, finish;
  int loop = 10000;
  int len = 1000000;
  float *a = (float *)malloc(sizeof(float) * len);
  float *b = (float *)malloc(sizeof(float) * len);
  float *d = (float *)malloc(sizeof(float) * len);
  for (int i = 0; i < len; i++) {
    // 随机生成数组
    a[i] = rand() * 1.8570f - 2.0360f;
    b[i] = rand() * 8.7680f - 6.3840f;
  }
  start = clock();
  for (int i = 0; i < loop; i++) {
    f(len, a, b, d);
  }
  finish = clock();
  double time = (double)(finish - start) / CLOCKS_PER_SEC;
  printf("time: %f s \n", time);
  a[0] = d[0]; // 调用结果 d 防止编译器将计算过程优化掉
  free(a);
  free(b);
  free(d);
  return 0;
}