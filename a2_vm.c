#include "a2_conf.h"
#include "a2_ir.h"
#include "a2_closure.h"
#include "a2_env.h"
#include "a2_error.h"
#include "a2_obj.h"

#define curr_ci    (vm_p->call_chain)
#define curr_pc    (curr_ci->pc)
#define curr_cls   (curr_ci->cls)
#define curr_ir    a2_closure_ir(curr_cls, curr_pc)
#define curr_op    (ir_gop(curr_ir))
#define curr_line  a2_closure_line(curr_cls, curr_pc)
   
#define vm_error(s)  do{a2_error("[vm error@%lu]: %s\n", curr_line, s);}while(0)


struct vm_callinfo{
	struct a2_closure* cls;

	size_t pc;

	struct vm_callinfo* next; 
	struct vm_callinfo* front;
};

struct  a2_vm{
	struct a2_env* env_p;

	struct vm_callinfo* call_chain;
};


static inline void a2_vm_run(struct a2_vm* vm_p);
static inline void _vm_load(struct a2_vm* vm_p);
static inline void _vm_loadnil(struct a2_vm* vm_p);
static inline void _vm_getgloval(struct a2_vm* vm_p);

struct a2_vm* a2_vm_new(struct a2_env* env_p){
	struct a2_vm* vm_p = (struct a2_vm*)malloc(sizeof(*vm_p));
	vm_p->env_p = env_p;
	return vm_p;
}


void a2_vm_free(struct a2_vm* vm_p){
	free(vm_p);
}

static inline void callinfo_new(struct a2_vm* vm_p, struct a2_closure* cls){
	assert(vm_p && cls);
	struct vm_callinfo* ci = (struct vm_callinfo*)malloc(sizeof(*ci));
	ci->cls = cls;
	ci->pc=0;
	ci->front = NULL;
	ci->next = vm_p->call_chain;
	if(vm_p->call_chain)
		vm_p->call_chain->front = ci;
	vm_p->call_chain = ci;
}

static inline void callinfo_free(struct a2_vm* vm_p){
	assert(vm_p->call_chain);
	struct vm_callinfo* b = vm_p->call_chain->next;
	free(vm_p->call_chain);
	if(b)
		b->front = NULL;
	vm_p->call_chain = b;
}

void a2_vm_load(struct a2_vm* vm_p, struct a2_closure* cls){
	assert(vm_p && cls);
	callinfo_new(vm_p, cls);
	a2_vm_run(vm_p);
}

// vm 
static void a2_vm_run(struct a2_vm* vm_p){
	for(;;){
		switch(curr_op){
			case LOAD:
				_vm_load(vm_p);
				break;
			case LOADNIL:
				_vm_loadnil(vm_p);
				break;
			case GETGLOBAL:
				_vm_getgloval(vm_p);
				break;
			default:
				assert(0);
		}
	}
}	


// load
static inline void _vm_load(struct a2_vm* vm_p){
	struct a2_obj* _des_obj = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	*_des_obj = *(a2_closure_const(curr_cls, ir_gbx(curr_ir)));
	curr_pc++;
}

// loadnil
static inline void _vm_loadnil(struct a2_vm* vm_p){
	int i;
	struct a2_obj* _obj = NULL;
	for(i=ir_ga(curr_ir); i<(ir_ga(curr_ir)+ir_gbx(curr_ir)); i++){
		_obj = a2_closure_arg(curr_cls, i);
		_obj->type = A2_TNIL;
	}
	curr_pc++;
}

//get global varable
static inline void _vm_getgloval(struct a2_vm* vm_p){
	int bx = ir_gbx(curr_ir);
	struct a2_obj* _obj = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _dobj = NULL;
	_dobj = (bx<0)?(a2_getgloval(vm_p->env_p, a2_closure_const(curr_cls, bx))):
				   (a2_closure_arg(curr_cls, bx));

	if(_dobj==NULL){
		char _buf[64] = {0};
		a2_error("[vm error@%lu]: the global \'%s\' is not find.\n", 
			curr_line, 
			obj2str(_obj, _buf, sizeof(_buf)));
	}
	*_dobj = *_obj;
}


