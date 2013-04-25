#ifndef _A2_XCLOSURE_H_
#define _A2_XCLOSURE_H_
#include "a2_conf.h"
#include "a2_ir.h"

#ifdef A2_JIT
#include "a2_jitcode.h"
#endif

struct obj_stack{
	struct a2_obj* stk_p;
	int top;
	int size;
};

enum upvaluex_type{
	uvx_reg,
	uvx_upvalue
};

// the upvalue index at a2_xclosure
struct upvaluex_idx{
	enum upvaluex_type uvx_type;
	union{
		int regs_idx;
		int upvalue_idx;
	}idx;
};

// the describe of closure
struct a2_xclosure{
	int refs;	// the count of reference 
	int params; // the count of parameters
	int regs;	// the count of register 

	// // intermediate representation chain
	ir* ir_chain; 
	size_t* lines;
	size_t len;
	size_t size;

	// include xclosures
	struct {
		struct a2_xclosure** xcls_chain;
		int cap;
		int size;
	}xcls_stack;

	// const varable stack
	struct obj_stack c_stack;

	// upvaluex chain
	struct {
		struct upvaluex_idx* upvaluex_chain;
		int len;
		int size;
	}upvaluex;

#ifdef A2_JIT
	// jit function 
	dynasm_func jit_func;
#endif
};

#define xcls_add_refs(xcls)			((xcls)->refs++)
void xcls_del_refs(struct a2_xclosure* xcls_p);

#define xcls_is_jit(xcls)			((xcls)->jit_func!=NULL)
#define xcls_call_jit(xcls)			((xcls)->jit_func)()
#define xcls_cur_uvx_count(xcls)	((xcls)->upvaluex.len)
#define xcls_const(xcls, idx)		(assert((idx)<0 && (-1-(idx))<(xcls)->c_stack.top), \
										&((xcls)->c_stack.stk_p[-1-(idx)]))
#define xcls_line(xcls, pc)			(assert((pc)<(xcls)->len), (xcls)->lines[pc])
#define xcls_xcls(xcls, idx)		(assert((idx)>=0 && (idx)<(xcls)->xcls_stack.cap), (xcls)->xcls_stack.xcls_chain[idx])

struct a2_xclosure* a2_xclosure_new();
void a2_xclosure_free(struct a2_xclosure* xcls);
inline size_t a2_xclosure_line(struct a2_xclosure* xcls, size_t pc);
inline void a2_xclosure_setparams(struct a2_xclosure* xcls, int params);

inline void a2_xclosure_setregs(struct a2_xclosure* xcls, int regs);

// IR op
size_t xclosure_add_ir(struct a2_xclosure* xcls, ir i, size_t line);
inline ir* xclosure_seek_ir(struct a2_xclosure* xcls, size_t idx);
inline size_t xclosure_curr_iraddr(struct a2_xclosure* xcls);
// const varable op
int xclosure_push_cstack(struct a2_env* env_p, struct a2_xclosure* xcls, struct a2_obj* obj);
struct  a2_obj* xclosure_at_cstack(struct a2_xclosure* xcls, int idx);
// xclosure op
int xclosure_push_xcls(struct a2_env* env_p, struct a2_xclosure* xcls, struct a2_xclosure* xcls_p);
// upvaluex op
int xclosure_push_upvaluex(struct a2_xclosure* xcls, enum upvaluex_type uvx_type, int idx);
inline struct a2_xclosure* a2_xclosure_upvaluexaddr(struct a2_xclosure* xcls, int up_idx, int* ret_idx);

// stack operation
void obj_stack_init(struct obj_stack* os_p);
void obj_stack_destory(struct obj_stack* os_p);
int obj_stack_add(struct obj_stack* os_p, struct a2_obj* obj_p);


// for test 
void dump_xclosure(struct a2_ir* ir_p, struct a2_xclosure* xcls);
#endif

