#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAYLEN 100000
int arrayA[ARRAYLEN];
int arrayB[ARRAYLEN];
int arrayC[ARRAYLEN];

// Prefetch function for ARM architecture
void inline Prefetch(int *ptr) {
    __asm__ volatile("prfm PLDL1KEEP, [%0, #128]" :: "r"(ptr));
}

void initialize_arrays() {
    for (int i = 0; i < ARRAYLEN; i++) {
        arrayA[i] = i;
        arrayB[i] = i * 2;
    }
}

int main() {
    initialize_arrays();

    int i;
    clock_t start, end;

    // Optimized version (using prefetch)
    start = clock();
    Prefetch(&arrayA[0]);
    Prefetch(&arrayB[0]);
    for (i = 0; i < ARRAYLEN - ARRAYLEN % 4; i += 4) {
        Prefetch(&arrayA[i + 4]);
        Prefetch(&arrayB[i + 4]);
        arrayC[i] = arrayA[i] * arrayB[i];
        arrayC[i + 1] = arrayA[i + 1] * arrayB[i + 1];
        arrayC[i + 2] = arrayA[i + 2] * arrayB[i + 2];
        arrayC[i + 3] = arrayA[i + 3] * arrayB[i + 3];
    }
    for (; i < ARRAYLEN; i++) {
        arrayC[i] = arrayA[i] * arrayB[i];
    }
    end = clock();
    double time_taken_with_prefetch = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;  // Convert to microseconds
    printf("Using prefetch time: %.0fus\n", time_taken_with_prefetch);

    return 0;
}
