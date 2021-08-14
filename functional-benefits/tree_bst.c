#include<stdio.h>
#include<stdlib.h>

struct tree {
	int ele;
	struct tree *left;
	struct tree *right;
};

//check the given tree is BST or not
int isbst(struct tree *root) {
	int a = 0, b = 0;

	if(root->left != NULL) {
		a = isbst(root->left);
	}
	else
		return 0;

	if(root->right != NULL) {
		b = isbst(root->right);
	}
	else
		return 0;

	if(root->ele >= root->right->ele || root->ele < root->left->ele)
		a = 1;
	return (a+b);
}



void main() {

	struct tree *root = (struct tree*)malloc(sizeof(struct tree));
	root->ele = 20;
	root->left = (struct tree*)malloc(sizeof(struct tree));
	root->right = (struct tree*)malloc(sizeof(struct tree));
	root->left->ele = 15;
	root->right->ele = 9;
	root->left->left = (struct tree*)malloc(sizeof(struct tree));
	root->left->right = (struct tree*)malloc(sizeof(struct tree));
	root->left->left->ele = 5;
	root->left->right->ele = 17;
	root->right->left = (struct tree*)malloc(sizeof(struct tree));
	root->right->left->ele = 22;

	int a;
	a = isbst(root);
	if(a == 0)
		printf("yes\n");
	else
		printf("no\n");
}
