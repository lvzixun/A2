#ifndef _A2_GC_H_
#define _A2_GC_H_
#include "a2_conf.h"

struct a2_gc;
struct a2_gcobj;
struct a2_closure;

struct a2_gc* a2_gc_new();
void a2_gc_free(struct a2_gc* gc_p);
void a2_gc_add(struct a2_gc* gc_p, struct a2_gcobj* gcobj);

// string 
inline void a2_gcobj_setstring(struct a2_gcobj* gcobj, char* a2_s);
struct a2_gcobj* a2_string2gcobj(char* a2_s);
inline char* a2_gcobj2string(struct a2_gcobj* gcobj);
void a2_gcobj_stringfree(struct a2_gcobj* gcobj);

// closure
struct a2_gcobj* a2_closure2gcobj(struct a2_closure* cls);

// nil 
struct a2_gcobj* a2_nil2gcobj();
void a2_gcobj_nilfree(struct a2_gcobj* gcobj);

#endif