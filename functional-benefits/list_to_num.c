#include<stdio.h>
#include<stdlib.h>

struct node {
	int ele;
	struct node *link;
};

//convert a list of digit to integer
int fun(struct node *next, int num) {
	if(next == NULL)
		return num;
	return fun(next->link, num*10+next->ele);
}



void main() {

	struct node *start = (struct node*)malloc(sizeof(struct node));
	struct node *next = start;
	next->ele = 1;

	for(int i = 0; i < 5; i++) {
		next->link  = (struct node*)malloc(sizeof(struct node));
		next = next->link;
		next->ele = i+2;
	}

	int a = fun(start, 0);
	printf("%d", a);
}
