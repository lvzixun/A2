#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_string.h"
#include "a2_closure.h"
#include <stdio.h>

struct a2_gcobj{
	int type;
	union{
		char* str;
		struct a2_closure* cls;
		void* obj;
	}value;
	struct a2_gcobj* next;
};


struct a2_gc{
	struct a2_gcobj* chain;
};

static void _gcobj_strMfree(struct a2_gcobj* gcobj);
static void _gcobj_clsMfree(struct a2_gcobj* gcobj);


struct a2_gc* a2_gc_new(){
	struct a2_gc* ret = (struct a2_gc*)malloc(sizeof(*ret));
	ret->chain = NULL;
	return ret;
}

void a2_gc_free(struct a2_gc* gc_p){
	assert(gc_p);
	struct a2_gcobj* p;
	while(gc_p->chain){
		p = gc_p->chain->next;
		switch(gc_p->chain->type){
			case A2_TSTRING:
				_gcobj_strMfree(gc_p->chain);
				break;
			case A2_TCLOSURE:
				_gcobj_clsMfree(gc_p->chain);
				break;
			default:
				free(gc_p->chain);
				break;
		}
		gc_p->chain = p;
	}
	free(gc_p);
}

void a2_gc_add(struct a2_gc* gc_p, struct a2_gcobj* gcobj){
	assert(gc_p);
	assert(gcobj);

	gcobj->next = gc_p->chain;
	gc_p->chain = gcobj;
}

inline void a2_gcobj_setstring(struct a2_gcobj* gcobj, char* a2_s){
	gcobj->type = A2_TSTRING;
	gcobj->value.str = a2_s;
}

struct a2_gcobj* a2_string2gcobj(char* a2_s){
	assert(a2_s);
	struct a2_gcobj* ret = a2_nil2gcobj();
	ret->type = A2_TSTRING;
	ret->value.str = a2_s;
	return ret;
}

struct a2_gcobj* a2_closure2gcobj(struct a2_closure* cls){
	assert(cls);
	struct a2_gcobj* ret = a2_nil2gcobj();
	ret->type = A2_TCLOSURE;
	ret->value.cls = cls;
	return ret;
}


struct a2_gcobj* a2_nil2gcobj(){
	struct a2_gcobj* ret = (struct a2_gcobj*)malloc(sizeof(*ret));
	ret->next = NULL;
	ret->type = A2_TSNIL;
	return ret;
}

void a2_gcobj_nilfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	free(gcobj);
}

inline char* a2_gcobj2string(struct a2_gcobj* gcobj){
	return gcobj->value.str;
}

// TODO: because gc , the gcobj only's mark.
void a2_gcobj_stringfree(struct a2_gcobj* gcobj){
	//mart it:
	
}

static void _gcobj_strMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TSTRING);
	a2_string_free(gcobj->value.str);
	free(gcobj); 
}

static void _gcobj_clsMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TCLOSURE);
	a2_closure_free(gcobj->value.cls);
	free(gcobj);
}
