#include "../include/traverse.h"

int inorder(const BiTreeNode* node, List* list) {
    if (bitree_is_eob(node)) {
        return 0;
    }

    if (!bitree_is_eob(bitree_left(node))) {
        if (inorder(bitree_left(node), list)) {
            return -1;
        }
    }

    if (list_ins_next(list, list_tail(list), bitree_data(node))) {
        return -1;
    }

    if (!bitree_is_eob(bitree_right(node))) {
        if (inorder(bitree_right(node), list)) {
            return -1;
        }
    }

    return 0;
}

int postorder(const BiTreeNode* node, List* list) {
    if (bitree_is_eob(node)) {
        return 0;
    }

    if (!bitree_is_eob(bitree_left(node))) {
        if (postorder(bitree_left(node), list)) {
            return -1;
        }
    }

    if (!bitree_is_eob(bitree_right(node))) {
        if (postorder(bitree_right(node), list)) {
            return -1;
        }
    }

    if (list_ins_next(list, list_tail(list), bitree_data(node))) {
        return -1;
    }

    return 0;
}
