#ifndef A2_CLOSURE_H_
#define A2_CLOSUTE_H_
#include "a2_conf.h"
#include "a2_ir.h"

struct a2_obj;
struct a2_closure;

struct a2_closure* a2_closure_new();
void a2_closure_free(struct a2_closure* cls);
inline void a2_closure_setparams(struct a2_closure* cls, int params);

inline size_t closure_add_ir(struct a2_closure* cls, ir i);
inline ir* closure_seek_ir(struct a2_closure* cls, size_t idx);
inline size_t closure_curr_iraddr(struct a2_closure* cls);

inline int closure_push_cstack(struct a2_closure* cls, struct a2_obj* obj);
inline struct  a2_obj* closure_at_cstack(struct a2_closure* cls, int idx);
inline int closure_push_clsstack(struct a2_closure* cls, struct a2_obj* obj);
inline int closure_push_ctnstack(struct a2_closure* cls, struct a2_obj* obj);

// light cp
inline int closure_push_upvalue(struct a2_closure* cls, struct a2_closure* cls_p, int arg_idx);

// dump
void dump_closure(struct a2_closure* cls);

#endif