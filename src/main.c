#include "../include/traverse.h"
#include <stdio.h>

void build_exptree(BiTree* tree) {
    /*

                ("*")
                /   \
               /     \
              /       \
             /         \
          ("/")        ("+")
           / \          / \
          /   \        /   \
      ("-")  ("32") ("23") ("17")
       / \
      /   \
  ("74")  ("10")

    */

    BiTreeNode* root;

    bitree_ins_left(tree, NULL, "*");
    root = bitree_root(tree);

    /* LHS */
    bitree_ins_left(tree, root, "/");
    bitree_ins_left(tree, root->left, "-");
    bitree_ins_right(tree, root->left, "32");
    bitree_ins_left(tree, root->left->left, "74");
    bitree_ins_right(tree, root->left->left, "10");

    /* RHS */
    bitree_ins_right(tree, root, "+");
    bitree_ins_left(tree, root->right, "23");
    bitree_ins_right(tree, root->right, "17");
}

int main(int argc, char** argv) {
    List tokens;
    ListElmt* element;
    BiTree expTree;

    list_init(&tokens, free);
    bitree_init(&expTree, free);

    build_exptree(&expTree);

    postorder(bitree_root(&expTree), &tokens);

    element = list_head(&tokens);
    while (element) {
        printf(" %s", (char* ) list_data(element));
        element = list_next(element);
    }

    puts(""); /* final newline */

    return 0;
}
