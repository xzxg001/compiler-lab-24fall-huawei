#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define ARRAY_LEN 80000
int data[ARRAY_LEN];
static struct timeval tm1;
static inline void start() { gettimeofday(&tm1, NULL); }
static inline void stop() {
  struct timeval tm2;
  gettimeofday(&tm2, NULL);
  unsigned long long t =
      1000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec) / 1000;
  printf("Average: %llu ms \n", t);
}
void swap_int(int *a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}
void bubble_sort(int *a, int n) {
  int i, t, s = 1;
  for (i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n - 1; j++)
      if (a[i] < a[j]) {
        swap_int(&a[i], &a[j]);
      }
  }
}
void sort_array() {
  printf("Bubble sorting array of %d elements\n", ARRAY_LEN);
  bubble_sort(data, ARRAY_LEN);
}
void get_data() {
  for (int i = 0; i < ARRAY_LEN; ++i) {
    data[i] = i;
  }
#ifdef __aarch64__
  __asm__ volatile("DMB SY");
#endif
}
int main() {
  get_data();
  start();
  sort_array();
  stop();
  return 0;
}