#include "a2_conf.h"
#include "a2_obj.h"
#include <stdio.h>

#define DEF_ARRAY_SIZE	32

struct a2_array{
	size_t len;
	size_t size;
	struct a2_obj* list;
};



struct a2_array* a2_array_new(){
	struct a2_array* ret = (struct a2_array*)malloc(sizeof(*ret));
	ret->list = (struct a2_obj*)malloc(sizeof(struct a2_obj)*DEF_ARRAY_SIZE);
	ret->size = DEF_ARRAY_SIZE;
	ret->len=0;
	return ret;
}


void a2_array_free(struct a2_array* array_p){
	if(array_p==NULL)
		return;
	free(array_p->list);
	free(array_p);
}

inline struct a2_obj* a2_array_next(struct a2_array* array_p, struct a2_obj* k){
	assert(k && k->type==A2_TNUMBER);
	if( (size_t)(k->value.number)>=array_p->len )
		return NULL;
	struct a2_obj* v = &(array_p->list[(size_t)(k->value.number)]);
	k->value.number = (a2_number)((size_t)(k->value.number) + 1);
	return v; 
}

inline struct a2_obj* a2_array_add(struct a2_array* array_p, struct a2_obj* obj){
	assert(obj);
	assert(array_p);
	if(array_p->len>=array_p->size){
		array_p->size *= 2;
		array_p->list = (struct a2_obj*)realloc(array_p->list, array_p->size*sizeof(struct a2_obj));
	}

	array_p->list[array_p->len++] = *obj;
	return obj;
}


struct a2_obj* a2_array_get(struct a2_array* array_p, struct a2_obj* k){
	assert(k->type==A2_TNUMBER);
	size_t idx=(size_t)(k->value.number);  
	if(idx<array_p->len)
		return NULL;
	
	return &(array_p->list[idx]);
}
