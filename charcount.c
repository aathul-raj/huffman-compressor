#include "charcount.h"
#include <stdio.h>
#include <string.h>

void count_chars(const char *buffer, size_t size, int *counts){
    memset(counts, 0, 128 * sizeof(int));
    for (size_t i = 0; i < size; i++) {
        unsigned char byte = (unsigned char)buffer[i];
        counts[byte]++;
    }
}