#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "charcount.h"
#include "printcounts.h"
#include "huff_tree.h"

unsigned char convert_bits_to_byte(char* bit_buffer){
    unsigned char byte = 0;

    for (int i = 0; i < 8; i++){
        if (bit_buffer[i] == '1'){
            byte |= (1 << (7 - i));
        }
    }

    return byte;
}

int main(int argc, char *argv[]){
    
    FILE *file;

    file = fopen(argv[1], "r");

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *counts_buffer = malloc(size);
    fread(counts_buffer, 1, size, file);
    fclose(file);

    int counts[256] = {};
    count_chars(counts_buffer, size, counts);
    free(counts_buffer);

    Node* root_node = build_tree(counts);

    // output file
    // output char count size - metadata
    // char count sizes

    FILE *fptr;
    fptr = fopen("output.txt", "wb");

    if (fptr == NULL){
        printf("error opening file!\n");
        return 1;
    }

    int32_t non_zero_count = 0;

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            non_zero_count++;
        }
    }

    fwrite(&non_zero_count, sizeof(int32_t), 1, fptr); // metadata

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            uint8_t char_code = (uint8_t)i;
            fwrite(&char_code, sizeof(uint8_t), 1, fptr); // char code
            fwrite(&counts[i], sizeof(int32_t), 1, fptr); // freq
        }
    }

    int32_t original_size = (int32_t)size;
    fwrite(&original_size, sizeof(int32_t), 1, fptr); // original file size

    file = fopen(argv[1], "r");

    int ch;

    int bit_count = 0;
    char bit_buffer[8];

    while ((ch = fgetc(file)) != EOF) {
        char* code = get_code(root_node, ch);
        if (code == NULL) {
            printf("Error: character '%c' is not in tree...", ch);
            return 1;
        }

        for (int i = 0; code[i] != '\0'; i++) {
            bit_buffer[bit_count++] = code[i];

            if (bit_count == 8){
                // convert to byte
                unsigned char byte = convert_bits_to_byte(bit_buffer);
                // write to file
                fwrite(&byte, sizeof(unsigned char), 1, fptr);
                bit_count = 0;
            }
        }
    }

    if (bit_count > 0) {
        while (bit_count < 8){
            bit_buffer[bit_count++] = '0';
        }
        unsigned char byte = convert_bits_to_byte(bit_buffer);
        fwrite(&byte, sizeof(unsigned char), 1, fptr);
    }

    fclose(fptr);

    free_tree(root_node);
    
    return 0;
}