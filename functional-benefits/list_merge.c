#include<stdio.h>
#include<stdlib.h>

struct node {
	int ele;
	struct node *link;
};

//two sorted lists are merged together to form a single sorted list
struct node* list_merge(struct node *list1, struct node *list2) {
	if(list1 == NULL)
		return list2;
	else if(list2 == NULL)
		return list1;

	if(list1->ele < list2->ele) {
		list1->link = list_merge(list1->link, list2);
		return list1;
	}
	else {
		list2->link = list_merge(list1, list2->link);
		return list2;
	}
}

void main() {
	struct node *list1 = (struct node*)malloc(sizeof(struct node));
	struct node *list2 = (struct node*)malloc(sizeof(struct node));
	struct node *next1 = list1;
	struct node *next2 = list2;
	next1->ele = 2;
	next2->ele = 1;

	for(int i = 4; i < 10; i+=2) {
		next1->link  = (struct node*)malloc(sizeof(struct node));
		next1 = next1->link;
		next1->ele = i;
	}
	for(int i = 3; i < 12; i+=2) {
		next2->link  = (struct node*)malloc(sizeof(struct node));
		next2 = next2->link;
		next2->ele = i;
	}
	struct node *m = list_merge(list1, list2);
	while(m != NULL) {
		printf("%d ", m->ele);
		m = m->link;
	}
}
