#ifndef _A2_GC_H_
#define _A2_GC_H_
#include "a2_conf.h"

struct a2_obj;
struct a2_map;
struct a2_array;
struct a2_gc;
struct a2_gcobj;
struct a2_closure;

struct a2_gc* a2_gc_new();
void a2_gc_free(struct a2_gc* gc_p);
struct a2_gcobj* a2_gc_add(struct a2_gc* gc_p, struct a2_gcobj* gcobj);

// string 
inline void a2_gcobj_setstring(struct a2_gcobj* gcobj, char* a2_s);
struct a2_gcobj* a2_string2gcobj(char* a2_s);
inline char* a2_gcobj2string(struct a2_gcobj* gcobj);
void a2_gcobj_stringfree(struct a2_gcobj* gcobj);

// closure
struct a2_gcobj* a2_closure2gcobj(struct a2_closure* cls);
inline struct a2_closure* a2_gcobj2closure(struct a2_gcobj* gcobj);

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
#endif

