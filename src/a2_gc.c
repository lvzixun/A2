#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_string.h"
#include "a2_closure.h"
#include "a2_array.h"
#include "a2_map.h"
#include "a2_env.h"
#include <stdio.h>

#define DEF_THRESHOLD_VALUE		128

#define  mask(m)	((m) & 0x02)


struct a2_gc{
	byte gc_flag;
	size_t gc_threshold;
	size_t gc_count;
	struct a2_env* env_p;
	struct a2_gcobj* chain;
};

static inline void a2_gc_clearit(struct a2_gc* gc_p, struct a2_gcobj* gcobj);

static void _gcobj_strMfree(struct a2_gc* gc_p, struct a2_gcobj* gcobj);
static void _gcobj_clsMfree(struct a2_gcobj* gcobj);
static void _gcobj_arrayMfree(struct a2_gcobj* gcobj);
static void _gcobj_mapMfree(struct a2_gcobj* gcobj);
static void _gcobj_upvalueMfree(struct a2_gcobj* gcobj);

static inline void _gc_mark_array(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m);
static inline void _gc_mark_map(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m);
static inline void _gc_mark_closure(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m);

struct a2_gc* a2_gc_new(struct a2_env* env_p){
	struct a2_gc* ret = (struct a2_gc*)malloc(sizeof(*ret));
	ret->env_p = env_p;
	ret->gc_threshold = DEF_THRESHOLD_VALUE;
	ret->chain = NULL;
	ret->gc_flag = a2_fail;
	ret->gc_count = 0;
	return ret;
}

void a2_gc_free(struct a2_gc* gc_p){
	assert(gc_p);
	struct a2_gcobj* p;

	while(gc_p->chain){
		p = gc_p->chain->next;
		a2_gc_clearit(gc_p, gc_p->chain);
		gc_p->chain = p;
	}
	free(gc_p);
}

void a2_gc_open(struct a2_gc* gc_p){
	gc_p->gc_flag = a2_true;
}

void a2_gc_close(struct a2_gc* gc_p){
	gc_p->gc_flag = a2_fail;
}

static inline void a2_gc_check(struct a2_gc* gc_p){
	if(gc_p->gc_flag==a2_true && gc_p->gc_count>gc_p->gc_threshold){
		a2_vmgc(gc_p->env_p);
		gc_p->gc_threshold = gc_p->gc_count*2;
		if(gc_p->gc_threshold<DEF_THRESHOLD_VALUE)
			gc_p->gc_threshold = DEF_THRESHOLD_VALUE;
	}
}

static inline void a2_gc_clearit(struct a2_gc* gc_p, struct a2_gcobj* gcobj){
	assert(gc_p->gc_count);
	gc_p->gc_count--;
	switch(gcobj->type){
		case A2_TSTRING:
			_gcobj_strMfree(gc_p, gcobj);
			break;
		case A2_TCLOSURE:
			_gcobj_clsMfree(gcobj);
			break;
		case A2_TARRAY:
			_gcobj_arrayMfree(gcobj);
			break;
		case A2_TMAP:
			_gcobj_mapMfree(gcobj);
			break;
		case _A2_TUPVALUE:
			_gcobj_upvalueMfree(gcobj);
			break;
		default:
			free(gcobj);
			break;
	}
}



void a2_gc_clear(struct a2_gc* gc_p){
	struct a2_gcobj* p = gc_p->chain;
	gc_p->chain = NULL;
	struct a2_gcobj* bp = NULL;
	struct a2_gcobj* fp = NULL;

	while(p){
		switch(p->mark){
			case mark_white:
				bp = p->next;
				a2_gc_clearit(gc_p, p);
				p = bp;
				break;
			case mark_black:
				if(fp) fp->next = p;
				fp = p;
				if(gc_p->chain == NULL)	gc_p->chain = p;
				p->mark = mark_white;
				p = p->next;
				break;
			case mark_blue:
				if(fp) fp->next = p;
				fp = p;
				if(gc_p->chain == NULL) gc_p->chain = p;
				p = p->next;
				break;
			default:
				assert(0);
				return;
		}
	}
	
	if(fp)
		fp->next = NULL;
}

void a2_gc_markit(struct a2_gc* gc_p, struct a2_obj* obj, enum gc_mark m){
	switch(obj_t(obj)){
		case A2_TARRAY:
			_gc_mark_array(gc_p, obj_vX(obj, obj), m);
			break;
		case A2_TMAP:
			_gc_mark_map(gc_p, obj_vX(obj, obj), m);
			break;
		case A2_TSTRING:
			obj_vX(obj, obj)->mark = m;
			break;
		case A2_TCLOSURE:
			_gc_mark_closure(gc_p, obj_vX(obj, obj), m);
			break;
	}
}


// mark array
static inline void _gc_mark_array(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m){
	assert(gcobj->type == A2_TARRAY);
	if(mask(gcobj->mark) == mask(m))
		return;

	gcobj->mark = m;
	struct a2_obj k = a2_nil2obj();
	struct a2_obj* vp = NULL;
	struct a2_array* array_p = a2_gcobj2array(gcobj);

	while(NULL != (vp=a2_array_next(array_p, &k))){
		a2_gc_markit(gc_p, vp, m);
	}
}

// mark map
static inline void _gc_mark_map(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m){
	assert(gcobj->type == A2_TMAP);
	if(mask(gcobj->mark) == mask(m))
		return;

	gcobj->mark = m;
	struct a2_obj k = a2_nil2obj();
	struct a2_obj* vp = NULL;
	struct a2_map* map_p = a2_gcobj2map(gcobj);
	while(NULL != (vp = a2_map_next(map_p, &k))){
		if(obj_t(&k)==A2_TSTRING){
			gcobj->mark = m;
		}
		a2_gc_markit(gc_p, vp, m);
	}

	// mark meta
	if(gcobj->meta){
		_gc_mark_map(gc_p, gcobj->meta, m);
	}
}

// mark closure
static inline void _gc_mark_closure(struct a2_gc* gc_p, struct a2_gcobj* gcobj, enum gc_mark m){
	assert(gcobj->type == A2_TCLOSURE);
	if(mask(gcobj->mark) == mask(m))
		return;

	gcobj->mark = m;

	size_t i;
	struct a2_closure* cls = a2_gcobj2closure(gcobj);

	// mark upvalue
	for(i=0; i<cls->uv_size; i++){
		if(cls->uv_chain[i].type == uv_gc){
			cls->uv_chain[i].v.uv_obj->mark = m;
			assert(cls->uv_chain[i].v.uv_obj->type == _A2_TUPVALUE);
			a2_gc_markit(gc_p, cls->uv_chain[i].v.uv_obj->value.uv, m);
		}
	}

	// mark const varable
	for(i=0; i<cls->xcls_p->c_stack.top; i++){
		if(obj_t(&(cls->xcls_p->c_stack.stk_p[i])) == A2_TSTRING){
			obj_vX(&(cls->xcls_p->c_stack.stk_p[i]), obj)->mark = m;
		}
	}
}

inline void a2_gc_mark(struct a2_gcobj* gcobj, enum gc_mark mark){
	gcobj->mark = mark;
}

inline void a2_gc_setmeta(struct a2_gcobj* gcobj, struct a2_gcobj* meta_obj){
	assert(gcobj->type==A2_TMAP);
	assert(meta_obj->type==A2_TMAP);
	gcobj->meta = meta_obj;
}

inline enum gc_mark a2_gc_getmark(struct a2_gcobj* gcobj){
	return gcobj->mark;
}

struct a2_gcobj* a2_gc_add(struct a2_gc* gc_p, struct a2_gcobj* gcobj){
	assert(gc_p);
	assert(gcobj);

	a2_gc_check(gc_p);
	gc_p->gc_count++;
	gcobj->next = gc_p->chain;
	gc_p->chain = gcobj;
	return gcobj;
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

struct a2_gcobj* a2_array2gcobj(struct a2_array* array){
	assert(array);
	struct a2_gcobj* ret = a2_nil2gcobj();
	ret->type = A2_TARRAY;
	ret->value.array = array;
	return ret;
}

struct a2_gcobj* a2_map2gcobj(struct a2_map* map){
	assert(map);
	struct a2_gcobj* ret = a2_nil2gcobj();
	ret->type = A2_TMAP;
	ret->value.map = map;
	return ret;
}

struct a2_gcobj* a2_upvalue2gcobj(struct a2_obj* obj){
	assert(obj);
	struct a2_gcobj* ret = a2_nil2gcobj();
	ret->type = _A2_TUPVALUE;
	ret->value.uv = (struct a2_obj*)malloc(sizeof(struct a2_obj));
	*(ret->value.uv) = *obj;
	return ret;
}

struct a2_gcobj* a2_nil2gcobj(){
	struct a2_gcobj* ret = (struct a2_gcobj*)malloc(sizeof(*ret));
	ret->next = NULL;
	ret->mark = mark_white;
	ret->type = A2_TNIL;
	return ret;
}

void a2_gcobj_nilfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	free(gcobj);
}

inline char* a2_gcobj2string(struct a2_gcobj* gcobj){
	assert(gcobj->type==A2_TSTRING);
	return gcobj->value.str;
}

inline struct a2_closure* a2_gcobj2closure(struct a2_gcobj* gcobj){
	assert(gcobj->type==A2_TCLOSURE);
	return gcobj->value.cls;
}

inline struct a2_array* a2_gcobj2array(struct a2_gcobj* gcobj){
	assert(gcobj->type==A2_TARRAY);
	return gcobj->value.array;
}

inline struct a2_map* a2_gcobj2map(struct a2_gcobj* gcobj){
	assert(gcobj->type==A2_TMAP);
	return gcobj->value.map;
}

inline struct a2_obj* a2_gcobj2upvalue(struct a2_gcobj* gcobj){
	assert(gcobj->type==_A2_TUPVALUE);
	return gcobj->value.uv;
}

static void _gcobj_strMfree(struct a2_gc* gc_p, struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TSTRING);
	a2_env_clear_itstring(gc_p->env_p, gcobj);
	a2_string_free(gcobj->value.str);
	free(gcobj); 
}

static void _gcobj_upvalueMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==_A2_TUPVALUE);
	free(gcobj->value.uv);
	free(gcobj);
}


static void _gcobj_clsMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TCLOSURE);
	a2_closure_free(gcobj->value.cls);
	free(gcobj);
}

static void _gcobj_arrayMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TARRAY);
	a2_array_free(gcobj->value.array);
	free(gcobj);
}

static void _gcobj_mapMfree(struct a2_gcobj* gcobj){
	assert(gcobj);
	assert(gcobj->type==A2_TMAP);
	a2_map_free(gcobj->value.map);
	free(gcobj);
}

// for test  
void dump_gc(struct a2_gc* gc_p){
	struct a2_gcobj* p = gc_p->chain;
	static char* ts[] = {
		// invalid object
		"_A2_TNULL",
	
		// public type
		"A2_TNIL",
		"A2_TSTRING",
		"A2_TNUMBER",
		"A2_TPOINT",
		"A2_TMAP",
		"A2_TARRAY",
		"A2_TCLOSURE",
		"A2_TCFUNCTION",
		"A2_TBOOL",

		//private type
		"_A2_TUINTEGER",
		"_A2_TADDR",
		"_A2_TUPVALUE",
		NULL
	};

	printf("count: %zd hold: %zd\n", gc_p->gc_count, gc_p->gc_threshold);
	while(p){
		printf("mark:%d type:%s %s addr:%p\n", (int)(p->mark), 
			ts[p->type], (p->type==A2_TSTRING)?(p->value.str):(""),  p);
		p = p->next;
	}

}


