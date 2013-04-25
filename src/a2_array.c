#include "a2_conf.h"
#include "a2_array.h"
#include "a2_obj.h"
#include <stdio.h>

#define DEF_ARRAY_SIZE	32


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
	assert(k);
	if(obj_t(k) == A2_TNIL){
		obj_setNum(k, -1);
	}

	if( (size_t)(obj_vNum(k) + 1)>=array_p->len )
		return NULL;
	struct a2_obj* v = &(array_p->list[(size_t)(obj_vNum(k)+1)]);
	a2_number number = (a2_number)((size_t)(obj_vNum(k))+1);
	obj_setNum(k, number);
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


inline struct a2_obj* a2_array_get(struct a2_array* array_p, struct a2_obj* k){
	assert(obj_t(k)==A2_TNUMBER);
	size_t idx=(size_t)(obj_vNum(k));  
	if(idx>=array_p->len)
		return NULL;
	
	return &(array_p->list[idx]);
}

inline size_t a2_array_len(struct a2_array* array_p){
	assert(array_p);
	return array_p->len;
}

