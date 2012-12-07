#include <stdlib.h>
#include <stdio.h>

static size_t mc = 0;

void* a2_malloc(size_t size){
	if(size==0) return NULL;
	mc++;
	return malloc(size);
}

void a2_free(void* p){
	if(p == NULL) return;
	mc--;
	free(p);
}

void mem_print(){
	printf("\nmem = %lu\n", mc);
}