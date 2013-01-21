#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t mc = 0;

struct mem_info {
	char* file;
	int line;
	void* p;
};

#define MEM_SIZE 1024
static struct mem_info _mi[MEM_SIZE];

static void _mem_add(void* p, char* file, int line){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==NULL){
			_mi[i].p = p;
			_mi[i].line = line;
			_mi[i].file = file;
			return;
		}
	}
}

static void _mem_del(void* p){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==p){
			_mi[i].p=NULL;
			return;
		}
	}
}

static void _mem_set(void* _p, void* p){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==_p){
			_mi[i].p = p;
		}
	}
}

void* a2_malloc(size_t size,  char* file, int line){
	if(size==0) return NULL;
	mc++;
	void* p = malloc(size);
	_mem_add(p, file, line);
	return p;
}

void* a2_calloc(size_t count, size_t size, char* file, int line){
	void* ret = malloc(size*count);
	_mem_add(ret, file, line);
	memset(ret, 0, size*count);
	mc++;
	return ret;
}

void* a2_realloc(void* p, size_t size){
	void* ret = realloc(p, size);
	_mem_set(p, ret);
	return ret;
}

void a2_free(void* p){
	if(p == NULL) return;
	mc--;
	_mem_del(p);
	free(p);
}


void mem_print(){
	printf("\nmem = %lu\n", mc);
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p){
			printf("[mem leak]: at %s file's line %d addr = %p\n", _mi[i].file, _mi[i].line, _mi[i].p);
		}
	}
}