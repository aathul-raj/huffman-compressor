#include "charcount.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test_hello(){
    int counts[256] = {};
    count_chars("hello", 5, counts);

    assert(counts['h'] == 1);
    assert(counts['e'] == 1);
    assert(counts['l'] == 2);
    assert(counts['o'] == 1);
    assert(counts['a'] == 0);

    printf("✓ test_hello passed!\n");
}

void test_empty(){
    int counts[256] = {};
    count_chars("", 0, counts);

    for (int i = 0; i < 256; i++){
        assert(counts[i] == 0);
    }

    printf("✓ test_empty passed!\n");
}

void test_special_chars() {
    int counts[256] = {};
    count_chars("hi\nbye", 6, counts);
    
    assert(counts['h'] == 1);
    assert(counts['i'] == 1);
    assert(counts['\n'] == 1);
    assert(counts['b'] == 1);
    assert(counts['y'] == 1);
    assert(counts['e'] == 1);
    
    printf("✓ test_special_chars passed!\n");
}

void test_numbers() {
    int counts[256] = {};
    count_chars("123", 3, counts);
    
    assert(counts['1'] == 1);
    assert(counts['2'] == 1);
    assert(counts['3'] == 1);
    
    printf("✓ test_numbers passed!\n");
}

void test_les_miserables() {
    int counts[256] = {};
    FILE *file;

    file = fopen("test.txt", "r");

    if (file == NULL){
        perror("error opening test.txt!");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size);
    fread(buffer, 1, size, file);
    fclose(file);

    count_chars(buffer, size, counts);

    assert(counts['X'] == 333);
    assert(counts['t'] == 223000);
    assert(counts['{'] == 1);

    free(buffer);

    printf("✓ test_les_miserables passed!\n");
}

int main(){
    printf("running charcount tests...\n");

    test_hello();
    test_empty();
    test_special_chars();
    test_numbers();
    test_les_miserables();

    printf("✓ all tests passed passed!\n");
    return 0;
}