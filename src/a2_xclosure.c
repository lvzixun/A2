#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_error.h"
#include "a2_xclosure.h"
#include "a2_vm.h"
#include <stdio.h>

#define DEF_STK_SIZE 32
#define DEF_UPVALUEX_SIZE 32
#define DEF_IR_SIZE 128
#define DEF_XCLS_SIZE 32


struct a2_xclosure* a2_xclosure_new(){
	struct a2_xclosure* xcls = (struct a2_xclosure*)malloc(sizeof(*xcls));

	xcls->params = 0;
	xcls->ir_chain = (ir*)malloc(sizeof(ir)*DEF_IR_SIZE);
	xcls->lines = (size_t*)malloc(sizeof(size_t)*DEF_IR_SIZE);
	xcls->size = DEF_IR_SIZE;
	xcls->len = 0;
	xcls->refs = 0;
	xcls->regs = 0;

	// init constent stack
	obj_stack_init(&(xcls->c_stack));

	// init clsource stack
	xcls->xcls_stack.size = DEF_XCLS_SIZE;
	xcls->xcls_stack.cap = 0;
	xcls->xcls_stack.xcls_chain = (struct a2_xclosure**)malloc(sizeof(struct a2_xclosure*)*DEF_XCLS_SIZE);

	// init upvaluex
	xcls->upvaluex.upvaluex_chain = (struct upvaluex_idx*)malloc(
		sizeof(struct upvaluex_idx)*DEF_UPVALUEX_SIZE);
	xcls->upvaluex.size = DEF_UPVALUEX_SIZE;
	xcls->upvaluex.len = 0;

	return xcls;
}

void a2_xclosure_free(struct a2_xclosure* xcls){
	assert(xcls);
	free(xcls->ir_chain);
	#ifdef A2_JIT
	// free jit code
	if(xcls->jit_func){
		a2_jitcode_free(xcls->jit_func);
		xcls->jit_func = NULL;
	}
	#endif
	
	// stack obj free
	obj_stack_destory(&(xcls->c_stack));
	free(xcls->xcls_stack.xcls_chain);
	// upvalue obj free
	free(xcls->upvaluex.upvaluex_chain);

	free(xcls->lines);
	free(xcls);
}

inline size_t a2_xclosure_line(struct a2_xclosure* xcls, size_t pc){
	assert(pc<xcls->len);
	return xcls->lines[pc];
}

inline void a2_xclosure_setparams(struct a2_xclosure* xcls, int params){
	assert(xcls);
	xcls->params = params;
}

inline void a2_xclosure_setregs(struct a2_xclosure* xcls, int regs){
	assert(regs>=0);
	xcls->regs = regs;
}

// IR OP
size_t xclosure_add_ir(struct a2_xclosure* xcls, ir i, size_t line){
	assert(xcls);
	// reszie
	if(xcls->len>=xcls->size){
		xcls->size *=2;
		xcls->ir_chain = (ir*)realloc(xcls->ir_chain, xcls->size*sizeof(ir));
		xcls->lines = (size_t*)realloc(xcls->lines, xcls->size*sizeof(size_t));
	}
	xcls->lines[xcls->len] = line;
	xcls->ir_chain[xcls->len] = i;
	return xcls->len++;
}

// seek ir
inline ir* xclosure_seek_ir(struct a2_xclosure* xcls, size_t idx){
	assert(idx<xcls->len);
	return &(xcls->ir_chain[idx]);
}

inline size_t xclosure_curr_iraddr(struct a2_xclosure* xcls){
	return xcls->len;
}

// cont varable stack op 
int xclosure_push_cstack(struct a2_env* env_p, struct a2_xclosure* xcls, struct a2_obj* obj){
	assert(xcls);
	assert(obj);
	if(xcls->c_stack.top>=BX_MAX)
		a2_error(env_p, e_overfllow_error,
		 "the constent stack is overfllow.\n");
	return obj_stack_add(&(xcls->c_stack), obj);
}

struct  a2_obj* xclosure_at_cstack(struct a2_xclosure* xcls, int idx){
	assert(xcls);
	assert(idx<0 && ((0-idx-1)<xcls->c_stack.top));
	return &(xcls->c_stack.stk_p[0-idx-1]);
}

// xclosure stack op
int xclosure_push_xcls(struct a2_env* env_p, struct a2_xclosure* xcls, struct a2_xclosure* xcls_p){
	assert(xcls && xcls_p);
	if(xcls->xcls_stack.cap>=BX_MAX)
		a2_error(env_p, e_overfllow_error,
		 "the gc stack from closure is overfllow.\n");
	if(xcls->xcls_stack.cap>=xcls->xcls_stack.size){
		xcls->xcls_stack.size *= 2;
		xcls->xcls_stack.xcls_chain = (struct a2_xclosure**)realloc(
			xcls->xcls_stack.xcls_chain,
			xcls->xcls_stack.size*sizeof(struct a2_xclosure));
	}
	int ret = xcls->xcls_stack.cap;
	xcls->xcls_stack.xcls_chain[xcls->xcls_stack.cap++] = xcls_p;
	assert(xcls_p->refs == 0);
	xcls_add_refs(xcls_p);
	return ret;
}

void xcls_del_refs(struct a2_xclosure* xcls_p){
	assert(xcls_p->refs >0);
	xcls_p->refs --;
	if(xcls_p->refs == 0){
		int i;
		for(i=0; i < xcls_p->xcls_stack.cap; i++){
			xcls_del_refs(xcls_p->xcls_stack.xcls_chain[i]);
		}
		a2_xclosure_free(xcls_p);
	}
}

// upvaluex op
int xclosure_push_upvaluex(struct a2_xclosure* xcls, enum upvaluex_type uvx_type, int idx){
	assert(xcls);
	assert(idx>=0);

	// resize
	if(xcls->upvaluex.len>=xcls->upvaluex.size){
		xcls->upvaluex.size *=2;
		xcls->upvaluex.upvaluex_chain = (void*)realloc(xcls->upvaluex.upvaluex_chain,
		  xcls->upvaluex.size*sizeof(*(xcls->upvaluex.upvaluex_chain)));
	}
	struct upvaluex_idx* uvx_idx = &(xcls->upvaluex.upvaluex_chain[xcls->upvaluex.len]);
	uvx_idx->uvx_type = uvx_type;
	switch(uvx_type){
		case uvx_reg:
			uvx_idx->idx.regs_idx = idx;
			break;
		case uvx_upvalue:
			uvx_idx->idx.upvalue_idx = idx;
			break;
		default:
			assert(0);
	}
	return xcls->upvaluex.len++;
}

// stack op
void obj_stack_init(struct obj_stack* os_p){
	os_p->stk_p = (struct a2_obj*)malloc(sizeof(struct a2_obj)*DEF_STK_SIZE);
	os_p->size = DEF_STK_SIZE;
	os_p->top =0;
}

 void obj_stack_destory(struct obj_stack* os_p){
	free(os_p->stk_p);
	os_p->stk_p = NULL;
	os_p->size=0;
	os_p->top = 0;
}

int obj_stack_add(struct obj_stack* os_p, struct a2_obj* obj_p){
	if(os_p->top>=os_p->size){
		os_p->size *= 2;
		os_p->stk_p = (struct a2_obj*)realloc(os_p->stk_p, os_p->size*sizeof(struct a2_obj));
	}
	os_p->stk_p[os_p->top] = *obj_p;
	return os_p->top++;
}

// for test 
void dump_upvalue(struct a2_xclosure* xcls){
	int i;
	static char* _ts[] = {
		"uvx_reg",
		"uvx_upvalue",
		NULL
	};
	for(i=0; i<xcls->upvaluex.len; i++){
		printf("upvalue@[%p] [%d] type=%s  idx= %d\n", 
			xcls, i, 
			_ts[xcls->upvaluex.upvaluex_chain[i].uvx_type],
			xcls->upvaluex.upvaluex_chain[i].idx.regs_idx);
	}
}

void dump_xclosure(struct a2_ir* ir_p, struct a2_xclosure* xcls){
	int i, j;
	assert(xcls);
	char buf[512] = {0};
	char* __cp = (xcls->params<0)?("+"):("");

	printf("\n\n----params=%d%s upvalue=%d const=%d regs=%d addr=%p-----\n", 
		(xcls->params<0)?(-1-xcls->params):(xcls->params), __cp, 
		xcls->upvaluex.len, xcls->c_stack.top, xcls->regs, xcls);
	dump_upvalue(xcls);
	for(i=0;i<xcls->len; i++){
		printf("<%zd>   [%d]   %s\n", xcls->lines[i],  i, ir2string(ir_p, xcls, xcls->ir_chain[i], buf, sizeof(buf)));
	}

	
	for(j=0; j<xcls->xcls_stack.cap; j++){
		dump_xclosure(ir_p, xcls->xcls_stack.xcls_chain[j]);
	}
}



