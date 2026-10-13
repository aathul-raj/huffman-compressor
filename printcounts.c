#include <stdio.h>

void print_counts(int *counts){
    printf("in print_counts\n");
    for (int i = 0; i < 128; i++) {
        if (counts[i] > 0) {
            if (i >= 32 && i < 127) {
                printf("%c count : %d\n", (char)i, counts[i]);
            } else {
                printf("[ASCII %d] count : %d\n", i, counts[i]);
            }
        }
    }
}