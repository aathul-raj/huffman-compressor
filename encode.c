
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huff_tree.h"
#include "charcount.h"

void build_encode_filename(const char* filename, char* output_buffer, size_t buffer_size) {
    const char* signature = "-shrinky-encoded.txt";

    const char* slash = strrchr(filename, '/');
    const char* dot = strrchr(filename, '.');
    size_t base_len = strlen(filename);
    if (dot && (!slash || dot > slash)) {
        base_len = (size_t)(dot - filename);
    }

    snprintf(output_buffer, buffer_size, "%.*s%s", (int)base_len, filename, signature);
}

long build_counts(char* filename, int* counts){
    FILE *file;

    file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size == 0){
        printf("cannot encode an empty file! exiting...\n");
        return 0;
    }

    char *counts_buffer = malloc(file_size);
    fread(counts_buffer, 1, file_size, file);
    fclose(file);

    count_chars(counts_buffer, file_size, counts);
    free(counts_buffer);

    return file_size;
}

unsigned char convert_bits_to_byte(char* bit_buffer){
    unsigned char byte = 0;

    for (int i = 0; i < 8; i++){
        if (bit_buffer[i] == '1'){
            byte |= (1 << (7 - i));
        }
    }

    return byte;
}

int encode(char* filename, int verbose) {
    FILE *input_file = fopen(filename, "r");

    int counts[256] = {};
    long file_size = build_counts(filename, counts);

    if (file_size == 0) {
        return 2;
    }

    Node* root_node = build_tree(counts);
    if (verbose) {
        printf("generated huffman tree!\n");
        print_tree(root_node, 0);
        printf("\n");
    }

    FILE *output_fptr;
    char output_name_buffer[256];

    build_encode_filename(filename, output_name_buffer, sizeof(output_name_buffer));

    output_fptr = fopen(output_name_buffer, "wb");

    if (output_fptr == NULL){
        printf("error opening file!\n");
        return 1;
    }

    if (verbose) {
        printf("encoding data...\n");
    }

    int32_t non_zero_count = 0;

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            non_zero_count++;
        }
    }

    fwrite(&non_zero_count, sizeof(int32_t), 1, output_fptr); // metadata

    if (verbose) {
        printf("wrote non_zero_count: %d\n", non_zero_count);
    }
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            uint8_t char_code = (uint8_t)i;
            fwrite(&char_code, sizeof(uint8_t), 1, output_fptr); // char code
            if (verbose) {
                printf("wrote char code for %c...\n", i);
            }
            fwrite(&counts[i], sizeof(int32_t), 1, output_fptr); // freq
            if (verbose) {
                printf("wrote freq: %d\n", counts[i]);
            }
        }
    }

    int32_t original_size = (int32_t)file_size;
    fwrite(&original_size, sizeof(int32_t), 1, output_fptr); // original file size

    if (verbose) {
        printf("wrote file size: %d\n", original_size);
    }

    int ch;

    int bit_count = 0;
    char bit_buffer[8];

    while ((ch = fgetc(input_file)) != EOF) {
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
                fwrite(&byte, sizeof(unsigned char), 1, output_fptr);
                if (verbose) {
                    printf("wrote encoded byte of data...\n");
                }
                bit_count = 0;
            }
        }
    }

    if (bit_count > 0) {
        while (bit_count < 8){
            bit_buffer[bit_count++] = '0';
        }
        unsigned char byte = convert_bits_to_byte(bit_buffer);
        fwrite(&byte, sizeof(unsigned char), 1, output_fptr);
        if (verbose) {
            printf("wrote overflow encoded byte of data...\n");
        }
    }

    if (verbose) {
        printf("encoded file! exiting...\n");
    }

    fclose(output_fptr);
    fclose(input_file);
    free_tree(root_node);

    return 0;
}