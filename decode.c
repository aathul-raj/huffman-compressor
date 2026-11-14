#include <stdio.h>
#include <stdint.h>
#include "huff_tree.h"
// #define BUFSZ 4

uint32_t bytes_to_int(unsigned char *bytes, int num_bytes){
    uint32_t val = 0;
    for (int i = 0; i < num_bytes; i++) {
        val |= (uint32_t)bytes[i] << (i * 8);
    }

    return val;
}

void convert_byte_to_bits(unsigned char byte, char* bit_buffer){
    int idx = 0;
    for (int i = 7; i >= 0; i--) {
        int bit = (byte >> i) & 1;
        bit_buffer[idx] = bit ? '1' : '0';
        idx += 1;
    }
}

void extract_bits(char buffers[][8], int left_buffer, int right_buffer, int left, int right, char *out){
    // this function should return any code across buffers as a char given the parameters
    int idx = 0;

    while (1) {
        out[idx++] = buffers[left_buffer][left];

        if ((left_buffer == right_buffer) && (left == right)){
            break;
        }

        left++;

        if (left >= 8) {
            left_buffer++;
            left = 0;
        }
    }

    out[idx] = '\0';
}

void get_next_buffer(FILE* input_file, char *out){
    // return next buffer in encoded file
    unsigned char compressed_byte[1] = {0};
    size_t compressed_byte_readsz = sizeof compressed_byte;

    fread(compressed_byte, sizeof *compressed_byte, compressed_byte_readsz, input_file);

    convert_byte_to_bits(compressed_byte[0], out);
}

int iterate_bit_buffers(FILE* input_file, long byte_size, int file_size, Node* root_node){
    FILE *output_fptr;
    output_fptr = fopen("output.txt", "wb");

    if (output_fptr == NULL){
        printf("error opening file!\n");
        return 1;
    }

    char buffers[byte_size][8];

    for (int i = 0; i < byte_size; i++){
        char buffer[8] = {0};
        get_next_buffer(input_file, buffer);

        for (int j = 0; j < 8; j++){
            buffers[i][j] = buffer[j];
            printf("%c", buffers[i][j]);
        }
        
    }
    printf("\n");

    int left_buffer = 0;

    int left = 0;
    int right = 0;

    int write_count = 0;

    for (int right_buffer = 0; right_buffer < byte_size; right_buffer++){
        // iterate through, get code, check if el.
        // if el, write to file
        // if el, left_buffer = right_buffer
        // if file size not reached yet, set l = r, r++ if r < 7 else r = 0

        for (int i = 0; i < 8; i++){
            printf("right_buffer: %d\n", right_buffer);
            printf("left_buffer: %d\n", left_buffer);
            printf("left: %d\n", left);
            printf("right: %d\n", right);
            char code[256 + 1] = {0};

            extract_bits(buffers, left_buffer, right_buffer, left, right, code); // code is now the code

            printf("current CODE: %s\n", code);

            char el = get_char(root_node, code);

            if (el) {
                fwrite(&el, sizeof(unsigned char), 1, output_fptr);
                write_count++;
                if (right == 7) {
                    left_buffer = right_buffer + 1;
                } else {
                    left_buffer = right_buffer;
                }
            }

            if (right < 7) {
                right++;
            } else {
                right = 0;
            }

            if (el) {
                left = right;
            }

            if (write_count == file_size) {
                printf("succesfully decoded entire file!\n");
                return 1;
            }
            printf("onto the next bit...\n");
        }
        printf("!!!!! onto the next buffer...\n");
    }

    return 0;
}

int decode(char *file){
    FILE *decoded_file;
    decoded_file = fopen("decoded.txt", "wb");

    if (decoded_file == NULL) {
        printf("error opening file...\n");
        return 1;
    }

    FILE *input_file = fopen(file, "rb");

    fseek(input_file, 0, SEEK_END);
    long encoded_file_size = ftell(input_file);
    rewind(input_file);

    int counts[256] = {};

    unsigned char non_zero_counts[4] = {0};
    size_t non_zero_counts_readsz = sizeof non_zero_counts;

    fread(non_zero_counts, sizeof *non_zero_counts, non_zero_counts_readsz, input_file);

    unsigned non_zero_counts_val = bytes_to_int(non_zero_counts, 4);

    for (int i = 0; i < non_zero_counts_val; i++) {
        // read char code
        unsigned char char_code[1] = {0};
        size_t char_code_readsz = sizeof char_code;

        fread(char_code, sizeof *char_code, char_code_readsz, input_file);
        unsigned char_code_val = bytes_to_int(char_code, 1);

        // read freq

        unsigned char freq[4] = {0};
        size_t freq_readsz = sizeof freq;

        fread(freq, sizeof *freq, freq_readsz, input_file);
        unsigned freq_val = bytes_to_int(freq, 4);

        counts[char_code_val] = freq_val;
    }

    Node* root_node = build_tree(counts);

    // file size

    unsigned char file_size[4] = {0};
    size_t file_size_readsz = sizeof file_size;

    fread(file_size, sizeof *file_size, file_size_readsz, input_file);
    unsigned file_size_val = bytes_to_int(file_size, 4);

    long byte_size = encoded_file_size - (sizeof(non_zero_counts_val) + (5 * non_zero_counts_val) + sizeof(file_size_val));

    iterate_bit_buffers(input_file, byte_size, file_size_val, root_node);

    printf("closing....\n");

    fclose(input_file);

    return 0;
}