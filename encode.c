
#include <stdint.h>
#include <stdio.h>
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

int encode(Node* root_node, int counts[256], char *argv[], long size) {
    FILE *output_fptr;
    output_fptr = fopen("output.txt", "wb");

    if (output_fptr == NULL){
        printf("error opening file!\n");
        return 1;
    }

    int32_t non_zero_count = 0;

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            non_zero_count++;
        }
    }

    fwrite(&non_zero_count, sizeof(int32_t), 1, output_fptr); // metadata

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            uint8_t char_code = (uint8_t)i;
            fwrite(&char_code, sizeof(uint8_t), 1, output_fptr); // char code
            fwrite(&counts[i], sizeof(int32_t), 1, output_fptr); // freq
        }
    }

    int32_t original_size = (int32_t)size;
    fwrite(&original_size, sizeof(int32_t), 1, output_fptr); // original file size

    FILE *input_file = fopen(argv[1], "r");

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
    }

    fclose(output_fptr);
    fclose(input_file);

    return 0;
}