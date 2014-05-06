#ifndef _A2_GC_H_
#define _A2_GC_H_
#include "a2_conf.h"
#include "a2_env.h"

enum gc_mark{
	mark_white = 0,		// can gc obj 
	mark_black = 2,		// used gc obj

	mark_blue = 3		// global gc obj
};

struct a2_gcobj{
	int type;
	byte mark;
	union{
		char* str;
		struct a2_closure* cls;
		struct a2_array* array;
		struct a2_map* map;
		struct a2_obj* uv;
		void* obj;
	}value;
	struct a2_gcobj* meta;  // meta data 
	struct a2_gcobj* next;
};


struct a2_obj;
struct a2_map;
struct a2_array;
struct a2_gc;
struct a2_closure;

struct a2_gc* a2_gc_new(struct a2_env* env_p);
void a2_gc_free(struct a2_gc* gc_p);
void a2_gc_open(struct a2_gc* gc_p);
void a2_gc_close(struct a2_gc* gc_p);
struct a2_gcobj* a2_gc_add(struct a2_gc* gc_p, struct a2_gcobj* gcobj);

// mark
void a2_gc_markit(struct a2_gc* gc_p, struct a2_obj* obj, enum gc_mark m);
inline void a2_gc_mark(struct a2_gcobj* gcobj, enum gc_mark mark);
inline enum gc_mark a2_gc_getmark(struct a2_gcobj* gcobj);

// clear
void a2_gc_clear(struct a2_gc* gc_p);

// string 
inline void a2_gcobj_setstring(struct a2_gcobj* gcobj, char* a2_s);
struct a2_gcobj* a2_string2gcobj(char* a2_s);
inline char* a2_gcobj2string(struct a2_gcobj* gcobj);
void a2_gcobj_stringfree(struct a2_gcobj* gcobj);

// closure
struct a2_gcobj* a2_closure2gcobj(struct a2_closure* cls);
inline struct a2_closure* a2_gcobj2closure(struct a2_gcobj* gcobj);

// meta
inline void a2_gc_setmeta(struct a2_gcobj* gcobj, struct a2_gcobj* meta_obj);

// array
struct a2_gcobj* a2_array2gcobj(struct a2_array* array); 
inline struct a2_array* a2_gcobj2array(struct a2_gcobj* gcobj);

// map
struct a2_gcobj* a2_map2gcobj(struct a2_map* map);
inline struct a2_map* a2_gcobj2map(struct a2_gcobj* gcobj);

// nil 
struct a2_gcobj* a2_nil2gcobj();
void a2_gcobj_nilfree(struct a2_gcobj* gcobj);

// upvalue
struct a2_gcobj* a2_upvalue2gcobj(struct a2_obj* obj);
inline struct a2_obj* a2_gcobj2upvalue(struct a2_gcobj* gcobj);

 // for test
 void dump_gc(struct a2_gc* gc_p);
#endif

