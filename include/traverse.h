#ifndef TRAVERSE_H
#define TRAVERSE_H

#include "bitree.h"
#include "list.h"

int inorder(const BiTreeNode* node, List* list);

int postorder(const BiTreeNode* node, List* list);

#endif
