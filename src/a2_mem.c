#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t mc = 0;

struct mem_info {
	char* file;
	int line;
	void* p;
};

static inline void* _a2_malloc(size_t size);
static inline void* _a2_calloc(size_t count, size_t size);

#ifdef _DEBUG_

#define MEM_SIZE 1024
static struct mem_info _mi[MEM_SIZE];

static void _mem_add(void* p, char* file, int line){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==NULL){
			_mi[i].p = p;
			_mi[i].line = line;
			_mi[i].line = line;
			return;
		}
	}
}

static void _mem_del(void* p){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==p){
			_mi[i].line = 0;
			_mi[i].p=NULL;
			_mi[i].line = 0;
			return;
		}
	}
}

static void _mem_set(void* _p, void* p){
	int i;
	for(i=0; i<MEM_SIZE; i++){
		if(_mi[i].p==_p){
			_mi[i].p = p;
			if(_p==NULL) mc++;
			return;
		}
	}
}

void* a2_malloc(size_t size,  char* file, int line){
	void* p =  _a2_malloc(size);
	_mem_add(p, file, line);
	return p;
}

void* a2_realloc(void* p, size_t size){
	void* ret = realloc(p, size);
	_mem_set(p, ret);
	return ret;
}

void* a2_calloc(size_t count, size_t size, char* file, int line){
	void* ret = _a2_calloc(count, size);
	_mem_add(ret, file, line);
	return ret;
}

#else
	void* a2_malloc(size_t size){
		return _a2_malloc(size);
	}

	void* a2_calloc(size_t count, size_t size){
		return _a2_calloc(count, size);
	}

	void* a2_realloc(void* p, size_t size){
		return realloc(p, size);
	}

#endif

static inline void* _a2_malloc(size_t size){
	mc++;
	return malloc(size);
}

static inline void* _a2_calloc(size_t count, size_t size){
	void* ret = malloc(size*count);
	memset(ret, 0, size*count);
	mc++;
	return ret;
}

void a2_free(void* p){
	if(p == NULL) return;
	mc--;
	#ifdef _DEBUG_
		_mem_del(p);
	#endif
	free(p);
}


void mem_print(){
	#ifdef _DEBUG_
		printf("\nmem = %zd\n", mc);
		int i;
		for(i=0; i<MEM_SIZE; i++){
			if(_mi[i].p){
				printf("[mem leak]: at %s file's line %d addr = %p\n", _mi[i].file, _mi[i].line, _mi[i].p);
			}
		}
	#endif
}

