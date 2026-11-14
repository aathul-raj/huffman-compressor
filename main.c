#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "charcount.h"
#include "printcounts.h"
#include "huff_tree.h"
#include "encode.h"
#include "decode.h"

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

    int encode_result_error = encode(root_node, counts, argv, size);

    if (encode_result_error) {
        printf("error encoding file!\n");
        return 1;
    } else {
        printf("succesfully encoded file!\n");
    }

    decode("output.txt");

    free_tree(root_node);
    
    return 0;
}