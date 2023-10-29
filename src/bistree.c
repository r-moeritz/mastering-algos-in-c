#include "../include/bistree.h"
#include <string.h>

static void destroy_right(BisTree* tree, BiTreeNode* node);

static void rotate_left(BiTreeNode** node) {
    BiTreeNode* left;
    BiTreeNode* grandchild;

    left = bitree_left(*node);

    if (((AvlNode*) bitree_data(left))->factor == AVL_LEFT_HEAVY) {
        /* Perform an LL rotation */
        bitree_left(*node) = bitree_right(left);
        bitree_right(left) = *node;

        ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
        ((AvlNode*) bitree_data(left))->factor = AVL_BALANCED;

        *node = left;
    }
    else {
        /* Perform an LR rotation */
        grandchild = bitree_right(left);
        bitree_right(left) = bitree_left(grandchild);
        bitree_left(grandchild) = left;
        bitree_left(*node) = bitree_right(grandchild);
        bitree_right(grandchild) = *node;

        switch (((AvlNode*) bitree_data((grandchild)))->factor) {
            case AVL_LEFT_HEAVY:
                ((AvlNode*) bitree_data(*node))->factor = AVL_RIGHT_HEAVY;
                ((AvlNode*) bitree_data(left))->factor = AVL_BALANCED;
                break;

            case AVL_BALANCED:
                ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                ((AvlNode*) bitree_data(left))->factor = AVL_BALANCED;
                break;

            case AVL_RIGHT_HEAVY:
                ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                ((AvlNode*) bitree_data(left))->factor = AVL_LEFT_HEAVY;
                break;
        }

        ((AvlNode*) bitree_data((grandchild)))->factor = AVL_BALANCED;
        *node = grandchild;
    }
}

static void rotate_right(BiTreeNode** node) {
    BiTreeNode* right;
    BiTreeNode* grandchild;

    right = bitree_right(*node);

    if (((AvlNode*) bitree_data(right))->factor == AVL_RIGHT_HEAVY) {
        /* Perform an RR rotation */
        bitree_right(*node) = bitree_left(right);
        bitree_left(right) = *node;

        ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
        ((AvlNode*) bitree_data(right))->factor = AVL_BALANCED;

        *node = right;
    }
    else {
        /* Perform an RL rotation */
        grandchild = bitree_left(right);
        bitree_left(right) = bitree_right(grandchild);
        bitree_right(grandchild) = right;
        bitree_right(*node) = bitree_left(grandchild);
        bitree_left(grandchild) = *node;

        switch (((AvlNode*) bitree_data(grandchild))->factor) {
            case AVL_LEFT_HEAVY:
                ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                ((AvlNode*) bitree_data(right))->factor = AVL_RIGHT_HEAVY;
                break;

            case AVL_BALANCED:
                ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                ((AvlNode*) bitree_data(right))->factor = AVL_BALANCED;
                break;

            case AVL_RIGHT_HEAVY:
                ((AvlNode*) bitree_data(*node))->factor = AVL_LEFT_HEAVY;
                ((AvlNode*) bitree_data(right))->factor = AVL_BALANCED;
                break;
        }

        ((AvlNode*) bitree_data(grandchild))->factor = AVL_BALANCED;
        *node = grandchild;
    }
}

static void destroy_left(BisTree* tree, BiTreeNode* node) {
    BiTreeNode** position;

    /* Don't allow destruction of an empty tree */
    if (!bitree_size(tree)) {
        return;
    }

    /* Determine where to destroy nodes */
    position = node ? &node->left : &tree->root;
    if (*position == NULL) {
        return;
    }

    destroy_left(tree, *position);
    destroy_right(tree, *position);

    if (tree->destroy) {
        /* Call a user-defined function to free dynamically allocated data */
        tree->destroy(((AvlNode*) (*position)->data)->data);
    }

    /* Free the AVL data in the node, then free the node itself */
    free((*position)->data);
    free(*position);
    *position = NULL;

    /* Adjust the size of the tree */
    tree->size -= 1;
}

static void destroy_right(BisTree* tree, BiTreeNode* node) {
    BiTreeNode** position;

    /* Don't allow destruction of an empty tree */
    if (!bitree_size(tree)) {
        return;
    }

    /* Determine where to destroy nodes */
    position = node ? &node->right : &tree->root;
    if (*position == NULL) {
        return;
    }

    destroy_left(tree, *position);
    destroy_right(tree, *position);

    if (tree->destroy) {
        /* Call a user-defined function to free dynamically allocated data */
        tree->destroy(((AvlNode*) (*position)->data)->data);
    }

    /* Free the AVL data in the node, then free the node itself */
    free((*position)->data);
    free(*position);
    *position = NULL;

    /* Adjust the size of the tree */
    tree->size -= 1;
}

static int insert(BisTree* tree, BiTreeNode** node, const void* data, int* balanced) {
    AvlNode* avl_node;

    int cmpval, retval;

    /* Insert data into the tree */
    if (bitree_is_eob(*node)) {
        /* Handle insertion into an empty tree */
        avl_node = (AvlNode*) malloc(sizeof(AvlNode));
        if (!avl_node) {
            return -1;
        }

        avl_node->factor = AVL_BALANCED;
        avl_node->hidden = 0;
        avl_node->data = (void*) data;

        return bitree_ins_left(tree, *node, avl_node);
    }
    else {
        /* Handle insertion into a tree that's not empty */
        cmpval = tree->compare(data, ((AvlNode*) bitree_data(*node))->data);
        if (cmpval < 0) {
            /* Move to the left */
            if (bitree_is_eob(bitree_left(*node))) {
                avl_node = (AvlNode*) malloc(sizeof(AvlNode));
                if (!avl_node) {
                    return -1;
                }

                avl_node->factor = AVL_BALANCED;
                avl_node->hidden = 0;
                avl_node->data = (void*) data;

                if (bitree_ins_left(tree, *node, avl_node)) {
                    return -1;
                }
                *balanced = 0;
            }
            else {
                retval = insert(tree, &bitree_left(*node), data, balanced);
                if (retval) {
                    return retval;
                }
            }

            /* Ensure the tree remains balanced */
            if (!(*balanced)) {
                switch (((AvlNode*) bitree_data(*node))->factor) {
                    case AVL_LEFT_HEAVY:
                        rotate_left(node);
                        *balanced = 1;
                        break;

                    case AVL_BALANCED:
                        ((AvlNode*) bitree_data(*node))->factor = AVL_LEFT_HEAVY;
                        break;

                    case AVL_RIGHT_HEAVY:
                        ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                        *balanced = 1;
                        break;
                }
            }
        } /* if (cmpval < 0) */
        else if (cmpval > 0) {
            /* Move to the right */
            if (bitree_is_eob(bitree_right(*node))) {
                avl_node = (AvlNode*) malloc(sizeof(AvlNode));
                if (!avl_node) {
                    return -1;
                }

                avl_node->factor = AVL_BALANCED;
                avl_node->hidden = 0;
                avl_node->data = (void*) data;

                if (bitree_ins_right(tree, *node, avl_node)) {
                    return -1;
                }
                *balanced = 0;
            }
            else {
                retval = insert(tree, &bitree_right(*node), data, balanced);
                if (retval) {
                    return retval;
                }
            }

            /* Ensure the tree remains balanced */
            if (!(*balanced)) {
                switch (((AvlNode*) bitree_data(*node))->factor) {
                    case AVL_LEFT_HEAVY:
                        ((AvlNode*) bitree_data(*node))->factor = AVL_BALANCED;
                        *balanced = 1;
                        break;

                    case AVL_BALANCED:
                        ((AvlNode*) bitree_data(*node))->factor = AVL_RIGHT_HEAVY;
                        break;

                    case AVL_RIGHT_HEAVY:
                        rotate_right(node);
                        *balanced = 1;
                        break;
                }
            }
        } /* if (cmpval > 0) */
        else {
            /* Handle finding a copy of the data */
            if (!((AvlNode*) bitree_data(*node))->hidden) {
                /* Do nothing since the data is in the tree & not hidden */
                return 1;
            }
            else {
                /* Insert the new data & mark it as not hidden */
                if (tree->destroy) {
                    /* Destroy the hidden data since it's being replaced */
                    tree->destroy(((AvlNode*) bitree_data(*node))->data);
                }

                ((AvlNode*) bitree_data(*node))->data = (void*) data;
                ((AvlNode*) bitree_data(*node))->hidden = 0;

                /* Don't rebalance since the tree structure is unchanged */
                *balanced = 1;
            }
        }
    }

    return 0;
}

static int hide(BisTree* tree, BiTreeNode* node, const void* data) {
    int cmpval, retval;

    if (bitree_is_eob(node)) {
        /* Data not found */
        return -1;
    }

    cmpval = tree->compare(data, ((AvlNode*) bitree_data(node))->data);
    if (cmpval < 0) {
        /* Move to the left */
        retval = hide(tree, bitree_left(node), data);
    }
    else if (cmpval > 0) {
        /* Move to the right */
        retval = hide(tree, bitree_right(node), data);
    }
    else {
        /* Mark the node as hidden */
        ((AvlNode*) bitree_data(node))->hidden = 1;
        retval = 0;
    }

    return retval;
}

static int lookup(BisTree* tree, BiTreeNode* node, void** data) {
    int cmpval, retval;

    if (bitree_is_eob(node)) {
        /* Data not found */
        return -1;
    }

    cmpval = tree->compare(*data, ((AvlNode*) bitree_data(node))->data);
    if (cmpval < 0) {
        /* Move to the left */
        retval = lookup(tree, bitree_left(node), data);
    }
    else if (cmpval > 0) {
        /* Move to the right */
        retval = lookup(tree, bitree_right(node), data);
    }
    else if (!((AvlNode*) bitree_data(node))->hidden) {
            /* Pass the data back from the tree */
        *data = ((AvlNode*) bitree_data(node))->data;
        retval = 0;
    }
    else {
        /* Data not found */
        retval = -1;
    }

    return retval;
}

void bistree_init(BisTree* tree,
                  int (*compare)(const void* key1, const void* key2),
                  void (*destroy)(void* data)) {
    bitree_init(tree, destroy);
    tree->compare = compare;
}

void bistree_destroy(BisTree* tree) {
    destroy_left(tree, NULL);
    memset(tree, 0, sizeof(BisTree));
}

int bistree_insert(BisTree* tree, const void* data) {
    int balanced = 0;
    return insert(tree, &bitree_root(tree), data, &balanced);
}

int bistree_remove(BisTree* tree, const void* data) {
    return hide(tree, bitree_root(tree), data);
}

int bistree_lookup(BisTree* tree, void** data) {
    return lookup(tree, bitree_root(tree), data);
}

