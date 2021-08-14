#include<stdio.h>
#include<stdlib.h>

struct node {
	
	int ele[3];
	struct node *link[4];
	int elefill;
	int linkfill;
	
};

struct stack {
	
	struct node *stacknode;
	struct stack *stacklink;
	
};

void insertele(struct node *point, int ele) {
	
	int elefill1 = point->elefill;
	
	if(point->ele[elefill1-1] < ele) {
	
		point->ele[elefill1] = ele;
	
	} else {
		
		point->ele[elefill1] = point->ele[elefill1-1];
		
		if(elefill1 == 2 && ele < point->ele[elefill1-2] && ele < point->ele[elefill1-1]) {
		
			point->ele[elefill1-1] = point->ele[elefill1-2];
			point->ele[elefill1-2] = ele;
		
		} else {
		
			point->ele[elefill1-1] = ele;
		
		}
	}
	
	point->elefill++;
	
}

void breaknode(struct node *point) {
	
	struct node *newleft = (struct node*)malloc(sizeof(struct node));
	struct node *newrite = (struct node*)malloc(sizeof(struct node));
	
	newleft->ele[0] = point->ele[0];
	newleft->elefill = 1;
	newleft->link[0] = point->link[0];
	newleft->link[1] = point->link[1];
	newleft->link[2] = NULL;
	newleft->link[3] = NULL;

	newrite->ele[0] = point->ele[2];
	newrite->elefill = 1;
	newrite->link[0] = point->link[2];
	newrite->link[1] = point->link[3];
	newrite->link[2] = NULL;
	newrite->link[3] = NULL;

	point->ele[0] = point->ele[1];
	point->elefill = 1;
	point->link[0] = newleft;
	point->link[1] = newrite;
	point->link[2] = NULL;
	point->link[3] = NULL;
	
	point->link[0]->linkfill = point->linkfill;
	point->link[1]->linkfill = point->linkfill;
	point->linkfill = 1;
	
}

void mergenode(struct node *point, struct stack *stack1, struct stack *stackp, int stacktop) {
	
	stackp = stack1;
	int sc = 0;
	
	while(sc < stacktop-1) {
		
		stackp = stackp->stacklink;
		sc++;
		
	}

	struct node *tnode = (struct node*)malloc(sizeof(struct node));
	
	if(stackp->stacknode->elefill == 3 && stacktop > 0) {
		
		breaknode(stackp->stacknode);		
		
		if(stackp->stacknode->ele[0] >= point->ele[0]) {
			
			tnode = stackp->stacknode->link[0];
		
		} else {
			
			tnode = stackp->stacknode->link[1];
			
		}
		
	} else {
		
		tnode = stackp->stacknode;
		
	}

	if(tnode->ele[tnode->elefill-1] < point->ele[0]) {
		
		tnode->link[tnode->elefill] = point->link[0];
		tnode->link[tnode->elefill+1] = point->link[1];
		tnode->ele[tnode->elefill] = point->ele[0];
		tnode->elefill++;
		
		free(point);
		
	} else {
		
		tnode->ele[tnode->elefill] = tnode->ele[tnode->elefill-1];
		tnode->link[tnode->elefill+1] = tnode->link[tnode->elefill];
		
		if(tnode->elefill == 2 && point->ele[0] < tnode->ele[0]) {
			
			tnode->ele[1] = tnode->ele[0];
			tnode->link[2] = tnode->link[1];
			tnode->ele[0] = point->ele[0]; //
			tnode->link[0] = point->link[0]; //
			tnode->link[1] = point->link[1]; //
			
		} else if (tnode->elefill == 2 && point->ele[0] > tnode->ele[0]) {
			
			tnode->ele[1] = point->ele[0];
			tnode->link[1] = point->link[0];
			tnode->link[2] = point->link[1];
		
		} else {
			
			tnode->ele[0] = point->ele[0]; //
			tnode->link[0] = point->link[0]; //
			tnode->link[1] = point->link[1]; //
			
		}
		
		tnode->elefill++;
		free(point);
		
	}
	
	stacktop--;
	point = stackp->stacknode;
	
	if(stacktop > 0) {
	
		mergenode(stackp->stacknode, stack1, stackp, stacktop);
	
	}
	
}

void printtree(struct node *root, struct node *point) {
	
	int i, j, k;
	printf("\n");
	
	for(k = 0; k < point->elefill; k++) {
		
		printf("%d.", point->ele[k]);
		
	}

	printf("\n");
	
	for(i = 0; i < 4; i++) {
		
		if(point->link[i] != NULL) {
		
			for(k = 0; k < point->link[i]->elefill; k++) {
				
				printf("%d.", point->link[i]->ele[k]);
			
			}
			
			printf("_ ");
	
		}
	
	}
	
	printf("\n");
	
	for(int j = 0; j < 4; j++) {
	        
		if(root->link[j] != NULL) {

			point = root->link[j];
	        
			for(i = 0; i < 4; i++) {
	                
				if(point->link[i] != NULL) {
	                        
					for(k = 0; k < point->link[i]->elefill; k++) {
						
						printf("%d.", point->link[i]->ele[k]);
					}
	                 
					printf("_ ");
	                
				}
	        
			}
	       
			printf("++");
	        
		}
	
	}

	printf("\n\n");

}




void main() {
	
	int arr[32] = {83,86,77,15,93,35,86,92,49,21,62,27,90,59,63,26,40,26,72,36,11,68,67,29,82,30,62,23,67,35,29,2};
	//int arr[20] = {49,47,45,43,41,39,37,35,33,31,29,27,25,23,21,19,17,15,13,11};
	int i = 0, c = 0, elefill1;
	
	struct node *root, *point, *new;
	struct stack *stack1, *stackp;
	stack1 = NULL;
	stackp = NULL;
	
	int stacktop = 0;
	root = NULL;
	point = NULL;

	while(c != 32) {

		printf("insert: %d\n", arr[c]);

		if(root == NULL) { //first element

			new = (struct node*)malloc(sizeof(struct node));
			new->ele[0] = arr[c];

			for(i = 0; i < 3; i++)
				new->link[i] = NULL;

			new->elefill = 1;
			new->linkfill = 0;
			root = new;
			point = root;

		} else {

			while(1) {

				if(point->linkfill == 0) {

					if( point->elefill != 3) {

						insertele(point, arr[c]);
						break;

					} else {

						breaknode(point);
						//printf("break node\n");
						//printtree(root, point);

						if(stacktop != 0) {

							mergenode(point, stack1, stackp, stacktop);
							//printf("merge node\n");
							//printtree(root, point);

						}

					}

					point = root;

				} else {

					if(stack1 == NULL) {

						stack1 = (struct stack*)malloc(sizeof(struct stack));
						stack1->stacknode = point;
						stack1->stacklink =NULL;
						stackp = stack1;

						stacktop++;

					} else {

						stackp->stacklink = (struct stack*)malloc(sizeof(struct stack));
						stackp->stacklink->stacknode = point;
						stackp->stacklink->stacklink = NULL;
						stackp = stackp->stacklink;

						stacktop++;

						if(point->elefill != 3) {

							stack1=stack1->stacklink;
							stacktop--;

						}

					}

					if(arr[c] <= point->ele[0])
						point = point->link[0];
					else if(arr[c] > point->ele[point->elefill-1])
						point = point->link[point->elefill];
					else if(point->elefill == 2)
						point = point->link[1];
					else if(point->elefill == 3 && arr[c] <= point->ele[1] )
						point = point->link[1];
					else
						point = point->link[2];

				}

			}

			stacktop = 0;
			stack1 = NULL;
			stackp = NULL;	

		}

		stacktop = 0;
		stack1 = NULL;
		stackp = NULL;

		point = root;
		c++;

		printtree(root, point);

	} //end of while(c)

	printtree(root, point);

} //end of main()
