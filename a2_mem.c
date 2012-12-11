#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t mc = 0;

void* a2_malloc(size_t size){
	if(size==0) return NULL;
	mc++;
	return malloc(size);
}

void* a2_calloc(size_t count, size_t size){
	void* ret = malloc(size*count);
	memset(ret, 0, size*count);
	return ret;
}

void a2_free(void* p){
	if(p == NULL) return;
	mc--;
	free(p);
}


void mem_print(){
	printf("\nmem = %lu\n", mc);
}