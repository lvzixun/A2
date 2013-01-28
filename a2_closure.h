#ifndef A2_CLOSURE_H_
#define A2_CLOSUTE_H_
#include "a2_conf.h"
#include "a2_ir.h"

struct a2_obj;
struct a2_closure;

struct obj_stack{
	struct a2_obj* stk_p;
	int top;
	int size;
};

// stack function
inline void obj_stack_init(struct obj_stack* os_p);
inline void obj_stack_destory(struct obj_stack* os_p);
inline int  obj_stack_add(struct obj_stack* os_p, struct a2_obj* obj_p);

// closure function 
struct a2_closure* a2_closure_new();
void a2_closure_free(struct a2_closure* cls);
inline void a2_closure_setparams(struct a2_closure* cls, int params);
inline int a2_closure_params(struct a2_closure* cls);

inline void a2_closure_setarg(struct a2_closure* cls, int args);
inline ir a2_closure_ir(struct a2_closure* cls, size_t idx);
inline struct a2_obj* a2_closure_upvalue(struct a2_closure* cls, int idx);
size_t a2_closure_line(struct a2_closure* cls, size_t pc);

inline size_t closure_add_ir(struct a2_closure* cls, ir i, size_t line);
inline ir* closure_seek_ir(struct a2_closure* cls, size_t idx);
inline size_t closure_curr_iraddr(struct a2_closure* cls);

inline int closure_push_cstack(struct a2_closure* cls, struct a2_obj* obj);
inline struct  a2_obj* closure_at_cstack(struct a2_closure* cls, int idx);
inline int closure_push_clsstack(struct a2_closure* cls, struct a2_obj* obj);
inline int closure_push_ctnstack(struct a2_closure* cls, struct a2_obj* obj);


// obj op
inline struct a2_obj* a2_closure_arg(struct a2_closure* cls, int idx);
inline struct a2_obj* a2_closure_const(struct a2_closure* cls, int idx);
inline struct a2_obj* a2_closure_container(struct a2_closure* cls, int idx);
inline struct a2_obj* a2_closure_cls(struct a2_closure* cls, int idx);

// light cp
inline int closure_push_upvalue(struct a2_closure* cls, struct a2_closure* cls_p, int arg_idx);

// dump 
void dump_closure(struct a2_ir* ir_p, struct a2_closure* cls);

#endif

