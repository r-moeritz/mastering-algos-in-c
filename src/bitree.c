#include "../include/bitree.h"
#include <string.h>

void bitree_init(BiTree* tree, void (*destroy)(void* data)) {
    tree->size = 0;
    tree->destroy = destroy;
    tree->root = NULL;
}

void bitree_destroy(BiTree* tree) {
    bitree_rem_left(tree, NULL);
    memset(tree, 0, sizeof(BiTree));
}

int bitree_ins_left(BiTree* tree, BiTreeNode* node, const void* data) {
    BiTreeNode* new_node;
    BiTreeNode** position;

    if (node == NULL) {
        /* Allow insertion only at the root of an empty tree */
        if (bitree_size(tree)) {
            return -1;
        }

        position = &tree->root;
    }
    else {
        /* Allow insertion only at the end of a branch */
        if (bitree_left(node)) {
            return -1;
        }

        position = &node->left;
    }

    /* Allocate storage for the node */
    new_node = (BiTreeNode*) malloc(sizeof(BiTreeNode));
    if (!new_node) {
        return -1;
    }

    /* Insert the node into the tree */
    new_node->data = (void*) data;
    new_node->left = NULL;
    new_node->right = NULL;
    *position = new_node;

    /* Adjust size of the tree */
    tree->size += 1;

    return 0;
}

int bitree_ins_right(BiTree* tree, BiTreeNode* node, const void* data) {
    BiTreeNode* new_node;
    BiTreeNode** position;

    if (!node) {
        /* Allow insertion only at the root of an empty tree */
        if (bitree_size(tree)) {
            return -1;
        }

        position = &tree->root;
    }
    else {
        /* Allow insertion only at the end of a branch */
        if (bitree_right(node)) {
            return -1;
        }

        position = &node->right;
    }

    /* Allocate storage for the node */
    new_node = (BiTreeNode*) malloc(sizeof(BiTreeNode));
    if (!new_node) {
        return -1;
    }

    /* Insert the node into the tree */
    new_node->data = (void*) data;
    new_node->left = NULL;
    new_node->right = NULL;
    *position = new_node;

    /* Adjust size of the tree */
    tree->size += 1;

    return 0;
}

void bitree_rem_left(BiTree* tree, BiTreeNode* node) {
    BiTreeNode** position;

    /* Don't allow removal from an empty tree */
    if (!bitree_size(tree)) {
        return;
    }

    position = node ? &node->left : &tree->root;
    if (!(*position)) {
        return;
    }

    /* Remove the nodes */
    bitree_rem_left(tree, *position);
    bitree_rem_right(tree, *position);

    if (tree->destroy) {
        /* Call a user-defined function to free dynamically allocated data */
        tree->destroy((*position)->data);
    }

    free(*position);
    *position = NULL;

    /* Adjust the size of the tree */
    tree->size -= 1;
}


void bitree_rem_right(BiTree* tree, BiTreeNode* node) {
    BiTreeNode** position;

    /* Don't allow removal from an empty tree */
    if (!bitree_size(tree)) {
        return;
    }

    position = node ? &node->right : &tree->root;
    if (!(*position)) {
        return;
    }

    /* Remove the nodes */
    bitree_rem_left(tree, *position);
    bitree_rem_right(tree, *position);

    if (tree->destroy) {
        /* Call a user-defined function to free dynamically allocated data */
        tree->destroy((*position)->data);
    }

    free(*position);
    *position = NULL;

    /* Adjust the size of the tree */
    tree->size -= 1;
}

int bitree_merge(BiTree* merge, BiTree* left, BiTree* right, const void* data) {
    /* Initialze the merged tree */
    bitree_init(merge, left->destroy);

    /* Insert data for the root node of the merged tree */
    if (bitree_ins_left(merge, NULL, data)) {
        bitree_destroy(merge);
        return -1;
    }

    /* Merge the two binary trees into a single tree */
    bitree_root(merge)->left = bitree_root(left);
    bitree_root(merge)->right = bitree_root(right);

    /* Adjust the size of the new tree */
    merge->size = merge->size + bitree_size(left) + bitree_size(right);

    /* Don't let the original trees access the merged nodes */
    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return 0;
}
