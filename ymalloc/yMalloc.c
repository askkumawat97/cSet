
// compile          = gcc ymalloc.c
// run              = ./a.out

// flag_area        = it store 0 for free and 1 for allocated.
// data_area        = used by user to store data.
// program_counter  = incremented by 1 for each step and traverse the flag area.
// available_flag   = 1-required free memory is available and 0-not available.
// free_counter     = counts available free memory.

#include<stdio.h>
#include<stdlib.h>

#define MAX 200
// this is available memory to use.
void* yalloc() {
	void *p = malloc(MAX);
	return p;
}

void* ymalloc(void *p, int size) {
	// first 4 bytes use for metadata.
	*(char*)p = 4; // the program_counter, initially starts with 4. 
	*(char*)(p+1) = 0; // available_flag.
	*(char*)(p+2) = 0; // count available free memory while traversing.
	*(char*)(p+3) = 0; // last allocated pointer.

	// while traversing for flag_area. 
	while(*(char*)p != (MAX-4)/2+3) {
		// at this point memory is free, increment free_counter.
		if(*(char*)(p+(*(char*)p)) == 0) {
			(*(char*)(p+2))++;
		}
		// if found allocated memory, reset available_counter value to zero, available_flag to 0.
		if(*(char*)(p+(*(char*)p)) != 0) {
			*(char*)(p+1) = 0;
			*(char*)(p+2) = 0;
		}
		// if required free memory available, set available_flag to 1 and stop.
		if(*(char*)(p+2) == size) {
			*(char*)(p+1) = 1;
			break;
		}
		// each iteration program_counter incremented by 1.
		(*(char*)p)++;
	}

	// if available_flag is 1, means required memory is available.
	if(*(char*)(p+1) == 1) {
		// store (size) at startind point, useful when it will free.
		*(char*)(p+((*(char*)p)-size)+1) = size;
		// while checking free memory, program_counter is present at the (p+size) position,
		// so set program_counter to (p-size) ie. the starting point.
		// also set all values to 1, ie. represents it is allocated.
		while(size != 1) {
			*(char*)(p+(*(char*)p)) = 1;
			(*(char*)p)--;
			size--;
		}
		// returning the pointer mapped to data_area.
		return (p+(MAX/2)-2+(*(char*)p));
	}
	else return (NULL);
}

void yfree(void *p1) {
	// the pointer is at data_area, demapped it to flag_area.
	p1 = p1-((MAX/2)-2);
	// set all flag_area to 0, ie. represent as free.
	// first place store the size.
	while(*(char*)p1 != 1) {
		*(char*)(p1+(*(char*)p1)-1) = 0;
		(*(char*)p1)--;
	}
	// first place is also reset.
	*(char*)(p1+(*(char*)p1)-1) = 0;
	p1 = NULL;
}




void main() {
	void *p;
	p = yalloc();


	//BELOW 
	///////////////////////////////////////
	char *ch1;
	int *ptr1;
	ch1 = ymalloc(p, 10*sizeof(char));
	ptr1 = ymalloc(p, 5*sizeof(int));

	int i1, a = 0;
	for(i1 = 0; i1 < 5; i1++)
		*(ptr1+i1) = (i1+1)*10;
	for(i1 = 0; i1 < 5; i1++)
		a = a+(*(ptr1+i1));
	printf("%d\n", a);
	///////////////////////////////////////

	//below code printf the internal flags and values
	////////////////////////////////////////////////////////////////
	//printf("%p\n%p\n%p\n%p\n\n",p,yp1,yp2,yp3);
	int i,r=4,k=0;
	for(i=0;i<MAX;i++){
		if(k%10==r)
			printf("\n");
		if(k==(MAX-4)/2+4) {
			printf("\n\n");
			r=((MAX-4)/2+4)%10;
		}
		if(*(char*)(p+k)<10)
			printf(" ");
		if(*(char*)(p+k)<100)
			printf(" ");
		
		printf("%d ",*(char*)(p+k));
		k++;
	}
	printf("\n\n");
	////////////////////////////////////////////////////////////////


	free(p);

}
