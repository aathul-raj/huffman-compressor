    #ifndef HUFF_TREE
    #define HUFF_TREE

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

    Node* build_tree(int *counts);

    void print_tree(Node* node, int depth);

    char* get_code(Node* node, char target_el);

    void free_tree(Node* node);

    #endif