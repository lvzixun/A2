#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_ir.h"
#include "a2_error.h"
#include <stdio.h>

#define DEF_STK_SIZE 32
#define DEF_UPVALUE_SIZE 32
#define DEF_IR_CHAIN_SIZE 64

#define DEF_IR_SIZE 128
#define DEF_ARG_SIZE 32

struct a2_closure{
	// intermediate representation chain
	ir* ir_chain;
	size_t len;
	size_t size;

	// const varabel stack
	struct {
		struct a2_obj* stk_p;
		int top;
		int size;
	}stack;

	// arg list
	struct {
		struct a2_obj* arg_p;
		int cap;
		int size;
	}arg;

	// upvalue  chain
	struct {
		struct {
			struct a2_closure* cls_p;
			int arg_idx;	
		}*upvalue_chain;
		int len;
		int size;
	}upvalue;
};

struct a2_closure* a2_closure_new(){
	struct a2_closure* ret = (struct a2_closure*)malloc(sizeof(*ret));
	// init ir chain
	ret->ir_chain = (ir*)malloc(sizeof(ir)*DEF_IR_SIZE);
	ret->size = DEF_IR_SIZE;
	ret->len = 0;
	// init stack
	ret->stack.stk_p = (struct a2_obj*)malloc(sizeof(struct a2_obj)*DEF_STK_SIZE);
	ret->stack.size = DEF_STK_SIZE;
	ret->stack.top = 0;
	// init upvalue
	ret->upvalue.upvalue_chain = (void*)malloc(sizeof(*(ret->upvalue.upvalue_chain))*DEF_UPVALUE_SIZE);
	ret->upvalue.size = DEF_UPVALUE_SIZE;
	ret->upvalue.len=0;

	return ret;
}

void a2_closure_free(struct a2_closure* cls){
	assert(cls);
	free(cls->ir_chain);
	//TODO: stack obj free
	free(cls->stack.stk_p);
	// TODO: upvalue obj free
	free(cls->upvalue.upvalue_chain);
	free(cls);
}

// IR OP
inline void closure_add_ir(struct a2_closure* cls, ir i){
	assert(cls);
	// reszie
	if(cls->len>=cls->size){
		cls->size *=2;
		cls->ir_chain = (ir*)realloc(cls->ir_chain, cls->size*sizeof(ir));
	}
	cls->ir_chain[cls->len++] = i;
}

// stack op
inline int closure_push_cstack(struct a2_closure* cls, struct a2_obj* obj){
	assert(cls);
	assert(obj);
	// resize
	if(cls->stack.top>=cls->stack.size){
		cls->stack.size *=2;
		cls->stack.stk_p = (struct a2_obj*)realloc(cls->stack.stk_p, 
			cls->stack.size*sizeof(struct a2_obj));
	}
	int ret = cls->stack.top;
	cls->stack.stk_p[(cls->stack.top)++] = *obj;
	return ret;
}

inline struct  a2_obj* closure_at_cstack(struct a2_closure* cls, int idx){
	assert(cls);
	assert(idx<0 && ((0-idx-1)<cls->stack.top));
	return &(cls->stack.stk_p[0-idx-1]);
}

// upvalue op
inline int closure_push_upvalue(struct a2_closure* cls, struct a2_closure* cls_p, int arg_idx){
	assert(cls);
	assert(cls_p);
	assert(arg_idx>=0);

	// resize
	if(cls->upvalue.len>=cls->upvalue.size){
		cls->upvalue.size *=2;
		cls->upvalue.upvalue_chain = (void*)realloc(cls->upvalue.upvalue_chain,
		  cls->upvalue.size*sizeof(*(cls->upvalue.upvalue_chain)));
	}
	cls->upvalue.upvalue_chain[(cls->upvalue.len)].cls_p = cls_p;
	int ret = cls->upvalue.len;
	cls_p->upvalue.upvalue_chain[(cls->upvalue.len)++].arg_idx = arg_idx;
	return ret;
}

// for test 
void dump_closure(struct a2_closure* cls){
	int i;
	assert(cls);
	char buf[512] = {0};
	printf("\n\n----arg=%d upvalue=%d addr=%p-----\n", cls->arg.cap, cls->upvalue.len, cls);
	for(i=0;i<cls->len; i++){
		printf("%s\n", ir2string(cls, cls->ir_chain[i], buf, sizeof(buf)));
	}
}


