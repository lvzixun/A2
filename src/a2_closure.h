#ifndef _A2_CLOSURE_H_
#define _A2_CLOSURE_H_
#include "a2_conf.h"
#include "a2_gc.h"
#include "a2_vm.h"
#include "a2_xclosure.h"

enum uv_type{
	uv_stack,
	uv_gc
};

struct a2_upvalue{
	enum uv_type type;
	struct a2_closure* cls;
	
	union{
		size_t sf_idx; // the index of stack frame.
		struct a2_gcobj* uv_obj; // the upvalue is added gc chain.
	}v;
};

struct a2_closure{
	struct a2_xclosure* xcls_p;

	// uped value stack frame  gc array
	struct{
		struct a2_gcobj** ud_gc_p;
		int size;
	}ud_sf_gc;

	// uped chain
	int ud_size;
	int ud_cap;
	struct a2_upvalue** uped_chain;

	// upvalue chain
	int uv_size;
	struct a2_upvalue uv_chain[1];
};

struct a2_closure* a2_closure_newrun(struct a2_xclosure* xcls);
struct a2_closure* a2_closure_new(struct vm_callinfo* ci, int idx);
void a2_closure_free(struct a2_closure* cls);

#define a2_closure_regscount(cls)		((cls)->xcls_p->regs)
#define a2_closure_ir(cls, pc)			(assert(pc>=0 && pc<(cls)->xcls_p->len), (cls)->xcls_p->ir_chain[pc])
#define a2_closure_ircount(cls)			((cls)->xcls_p->len)
#define a2_closure_const(cls, idx)  	xcls_const((cls->xcls_p), (idx))
#define a2_closure_line(cls, pc)		xcls_line((cls)->xcls_p, (pc))
#define a2_closure_cls(cls, idx)		xcls_xcls((cls)->xcl_p, idx)
#define a2_closure_upvalue(cls, idx)	(assert(idx>=0 && idx<(cls)->uv_size), &((cls)->uv_chain[idx]))

inline int a2_closure_params(struct a2_closure* cls);
void a2_closure_return(struct a2_closure* cls, struct a2_env* env_p);
#endif

