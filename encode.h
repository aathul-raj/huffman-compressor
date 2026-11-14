#ifndef ENCODE_H
#define ENCODE_H
#include "huff_tree.h"

int encode(Node* root_node, int counts[256], char *argv[], long size);

#endif