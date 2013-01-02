#ifndef A2_CLOSURE_H_
#define A2_CLOSUTE_H_
#include "a2_conf.h"
#include "a2_ir.h"

struct a2_closure;

struct a2_closure* a2_closure_new();
void a2_closure_free(struct a2_closure* cls);

inline void closure_add_ir(struct a2_closure* cls, ir i);

inline int closure_push_cstack(struct a2_closure* cls, struct a2_obj* obj);
inline void closure_pop_cstack(struct a2_closure* cls, int cap);

// light cp
inline int closure_push_upvalue(struct a2_closure* cls, struct a2_closure* cls_p, int arg_idx);
#endif