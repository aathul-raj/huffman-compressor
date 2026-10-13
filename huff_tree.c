#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Node {
    char el;
    int freq;
    char *code;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node **nodes;
    int count;
    int size;
} NodeList;

NodeList init_node_list(int *counts){
    NodeList temp_nodelist = {.nodes = malloc(256 * sizeof(Node*)), .size = 256};
    for (int i = 0; i < 256; i++){
        if (counts[i] > 0) {
            temp_nodelist.nodes[i] = malloc(sizeof(Node));
            temp_nodelist.nodes[i]->el = i;
            temp_nodelist.nodes[i]->freq = counts[i];
            temp_nodelist.nodes[i]->left = NULL;
            temp_nodelist.nodes[i]->right = NULL; 
            temp_nodelist.nodes[i]->code = NULL;
            temp_nodelist.count++;
        }
    }
    
    NodeList nodelist = {.nodes = malloc((temp_nodelist.count * 2 - 1) * sizeof(Node*)), .count = temp_nodelist.count, .size = temp_nodelist.count};
    int idx = 0;

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0){
            nodelist.nodes[idx] = temp_nodelist.nodes[i];
            idx++;
        }
        // might need to free the rest? not sure
    }

    free(temp_nodelist.nodes);

    return nodelist;
}

Node* find_min_node(NodeList *nodelist){
    // modifies nodelist so that the min node sits at nodelist[nodelist.count]
    Node min = {.freq = __INT_MAX__};
    int idx = 0;

    for (int i = 0; i < nodelist->count; i++){
        Node curr_node = *nodelist->nodes[i];
        if (curr_node.freq <= min.freq){
            min.el = curr_node.el;
            min.freq = curr_node.freq;
            idx = i;
        }
    }

    Node* temp = nodelist->nodes[idx];
    nodelist->nodes[idx] = nodelist->nodes[nodelist->count - 1];
    nodelist->nodes[nodelist->count - 1] = temp;
    
    nodelist->count--;
    return nodelist->nodes[nodelist->count];
}

void assign_code(Node* node, char* code, int depth){
    if (node->left == NULL && node->right == NULL){
        code[depth] = '\0';
        node->code = malloc(depth + 1);
        memcpy(node->code, code, depth + 1);
        printf("%c has code : %s\n", node->el, code);
        return;
    }

    if (node->left){
        code[depth] = '0';
        assign_code(node->left, code, depth + 1);
    }

    if (node->right){
        code[depth] = '1';
        assign_code(node->right, code, depth + 1);
    }
}

Node* build_tree(int *counts){
    NodeList nodelist = init_node_list(counts);
    
    while (nodelist.count > 1) {
        // loop through nodelist and get 2 smallest mins
        // remove (they will each sit at last 2 positions of nodelist)
        Node* min_left = find_min_node(&nodelist);
        Node* min_right = find_min_node(&nodelist);

        // create parent, insert back
        Node *parent = malloc(sizeof(Node));
        parent->el = '\0';
        parent->left = min_left;
        parent->right = min_right;
        parent->freq = min_left->freq + min_right->freq;

        nodelist.nodes[nodelist.count] = parent;
        nodelist.count++;
    }

    Node* root = nodelist.nodes[0];
    assign_code(root, malloc(256), 0);
    free(nodelist.nodes);

    return root;
}

char* get_code(Node* node, char target_el){
    if (node == NULL) {
        return NULL;
    }

    if (node->el == target_el){
        return node->code;
    }

    if (node->left) {
        char* result = get_code(node->left, target_el);
        if (result) return result;
    }

    if (node->right) {
        char* result = get_code(node->right, target_el);
        if (result) return result;
    }

    return NULL;
}

void print_tree(Node* node, int depth){
    for (int i = 0; i < depth; i++){
        printf("  "); // 2 spaces per level
    }

    if (node->el == '\0') {
        printf("(internal) freq=%d\n", node->freq);
    } else {
        printf("'%c' freq=%d\n", node->el, node->freq);
    }
    
    if (node->left){
        print_tree(node->left, depth + 1);
    }

    if (node->right){
        print_tree(node->right, depth + 1);
    }

    return;
}

void free_tree(Node* node){
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node->code);
    free(node);
}