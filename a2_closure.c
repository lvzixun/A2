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

struct obj_stack{
	struct a2_obj* stk_p;
	int top;
	int size;
};

struct a2_closure{
	int params;
	// intermediate representation chain
	ir* ir_chain;
	size_t* lines;
	size_t len;
	size_t size;
	
	// closure obj stack
	struct obj_stack cls_stack;

	// container obj stack 
	struct obj_stack ctn_stack;

	// const varabel stack
	struct obj_stack c_stack;

	// arg list
	struct {
		struct a2_obj* arg_p;
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

static inline void _obj_stack_init(struct obj_stack* os_p);
static  inline void _obj_stack_destory(struct obj_stack* os_p);
static inline int _obj_stack_add(struct obj_stack* os_p, struct a2_obj* obj_p);

struct a2_closure* a2_closure_new(){
	struct a2_closure* ret = (struct a2_closure*)malloc(sizeof(*ret));
	// init ir chain
	ret->params = 0;
	ret->ir_chain = (ir*)malloc(sizeof(ir)*DEF_IR_SIZE);
	ret->lines = (size_t*)malloc(sizeof(size_t)*DEF_IR_SIZE);
	ret->size = DEF_IR_SIZE;
	ret->len = 0;
	ret->arg.arg_p = NULL;
	ret->arg.size = 0;
	// init constent stack
	_obj_stack_init(&(ret->c_stack));
	// init cls stack
	_obj_stack_init(&(ret->cls_stack));
	// init container stack
	_obj_stack_init(&(ret->ctn_stack));
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
	_obj_stack_destory(&(cls->c_stack));
	_obj_stack_destory(&(cls->cls_stack));
	_obj_stack_destory(&(cls->ctn_stack));
	// TODO: upvalue obj free
	free(cls->upvalue.upvalue_chain);
	free(cls->arg.arg_p);
	free(cls->lines);
	free(cls);
}

size_t a2_closure_line(struct a2_closure* cls, size_t pc){
	assert(pc<cls->len);
	return cls->lines[pc];	
}

inline void a2_closure_setarg(struct a2_closure* cls, int args){
	assert(cls->arg.arg_p==NULL && cls->arg.size==0);
	cls->arg.size = args;
	cls->arg.arg_p = (struct a2_obj*)malloc(sizeof(struct a2_obj)*args);
}

inline void a2_closure_setparams(struct a2_closure* cls, int params){
	assert(cls);
	cls->params = params;
}

inline ir a2_closure_ir(struct a2_closure* cls, size_t idx){
	assert(idx<cls->len);
	return cls->ir_chain[idx];
}

inline struct a2_obj* a2_closure_arg(struct a2_closure* cls, int idx){
	assert(idx>=0 && idx<cls->arg.size);
	return &(cls->arg.arg_p[idx]);
}

inline struct a2_obj* a2_closure_const(struct a2_closure* cls, int idx){
	assert(idx<0 && (-1-idx)<cls->c_stack.top);
	return &(cls->c_stack.stk_p[-1-idx]);
}

inline struct a2_obj* a2_closure_container(struct a2_closure* cls, int idx){
	assert(idx>=0 && idx<cls->ctn_stack.top);
	return &(cls->ctn_stack.stk_p[idx]);
}

inline struct a2_obj* a2_closure_cls(struct a2_closure* cls, int idx){
	assert(idx>=0 && idx<cls->cls_stack.top);
	return &(cls->cls_stack.stk_p[idx]);
}

static inline void _obj_stack_init(struct obj_stack* os_p){
	os_p->stk_p = (struct a2_obj*)malloc(sizeof(struct a2_obj)*DEF_STK_SIZE);
	os_p->size = DEF_STK_SIZE;
	os_p->top =0;
}

static  inline void _obj_stack_destory(struct obj_stack* os_p){
	free(os_p->stk_p);
	os_p->stk_p = NULL;
	os_p->size=0;
	os_p->top = 0;
}

static inline int _obj_stack_add(struct obj_stack* os_p, struct a2_obj* obj_p){
	if(os_p->top>=os_p->size){
		os_p->size *= 2;
		os_p->stk_p = (struct a2_obj*)realloc(os_p->stk_p, os_p->size*sizeof(struct a2_obj));
	}
	os_p->stk_p[os_p->top] = *obj_p;
	return os_p->top++;
}

// IR OP
inline size_t closure_add_ir(struct a2_closure* cls, ir i, size_t line){
	assert(cls);
	// reszie
	if(cls->len>=cls->size){
		cls->size *=2;
		cls->ir_chain = (ir*)realloc(cls->ir_chain, cls->size*sizeof(ir));
		cls->lines = (size_t*)realloc(cls->lines, cls->size*sizeof(size_t));
	}
	cls->lines[cls->len] = line;
	cls->ir_chain[cls->len] = i;
	return cls->len++;
}

// seek ir
inline ir* closure_seek_ir(struct a2_closure* cls, size_t idx){
	assert(idx<cls->len);
	return &(cls->ir_chain[idx]);
}

inline size_t closure_curr_iraddr(struct a2_closure* cls){
	return cls->len;
}

// stack op
inline int closure_push_cstack(struct a2_closure* cls, struct a2_obj* obj){
	assert(cls);
	assert(obj);
	if(cls->c_stack.top>=BX_MAX)
		a2_error("the constent stack is overfllow.\n");
	return _obj_stack_add(&(cls->c_stack), obj);
}

inline struct  a2_obj* closure_at_cstack(struct a2_closure* cls, int idx){
	assert(cls);
	assert(idx<0 && ((0-idx-1)<cls->c_stack.top));
	return &(cls->c_stack.stk_p[0-idx-1]);
}

inline int closure_push_clsstack(struct a2_closure* cls, struct a2_obj* obj){
	if(cls->cls_stack.top>=BX_MAX)
		a2_error("the gc stack from closure is overfllow.\n");
	return _obj_stack_add(&(cls->cls_stack), obj);
}

inline int closure_push_ctnstack(struct a2_closure* cls, struct a2_obj* obj){
	if(cls->ctn_stack.top>=BX_MAX)
		a2_error("the gc stack from closure is overfllow.\n");
	return _obj_stack_add(&(cls->ctn_stack), obj);
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
void dump_closure(struct a2_ir* ir_p, struct a2_closure* cls){
	int i, j;
	assert(cls);
	char buf[512] = {0};
	char* __cp = (cls->params<0)?("+"):("");

	printf("\n\n----params=%d%s upvalue=%d const=%d addr=%p-----\n", 
		(cls->params<0)?(-1-cls->params):(cls->params), __cp, 
		cls->upvalue.len, cls->c_stack.top, cls);
	for(i=0;i<cls->len; i++){
		printf("<%lu>   [%d]   %s\n", cls->lines[i],  i, ir2string(ir_p, cls, cls->ir_chain[i], buf, sizeof(buf)));
	}

	
	for(j=0; j<cls->cls_stack.top; j++){
		assert(cls->cls_stack.stk_p[j].type==A2_TCLOSURE);
		dump_closure(ir_p, a2_gcobj2closure(cls->cls_stack.stk_p[j].value.obj));
	}
}




