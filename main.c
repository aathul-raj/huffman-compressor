#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"

int encode_path(char* filename, int verbose) {
    int encode_result_error = encode(filename, verbose);

    if (encode_result_error == 2){ // empty file, graceful exit
        return 0;
    } else if (encode_result_error) {
        printf("error encoding file!\n");
        return 1;
    } else {
        return 0;
    }
}

void file_size(char* filename) {
    FILE *file;

    file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    printf("file size of %s : %ld (bytes)\n", filename, file_size);
}


int main(int argc, char *argv[]){
    int verbose = 0;
    int encode = 1;
    char* filename = argv[1]; // we expect the first argument to be the file

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        if (strcmp(arg, "-v") == 0) {
            verbose = 1;
        } else if (strcmp(arg, "-s") == 0) {
            file_size(filename);
            return 0;
        } else if (strcmp(arg, "unshrink") == 0) {
            decode(filename, verbose);
            encode = 0;
        }
    }

    if (encode) {
        encode_path(filename, verbose);
    }
    
    return 0;
}