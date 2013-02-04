#ifndef _A2_XCLOSURE_H_
/*
	the head file is for internal use.  mostly int the vm modle.
*/

#define _A2_XCLOSURE_H_
#include "a2_conf.h"
#include "a2_closure.h"

struct upvalue_idx{
	struct a2_closure* cls_p;
	int arg_idx;
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
		struct upvalue_idx* upvalue_chain;
		int len;
		int size;
	}upvalue;
};


// the operating args of closure. the macro is for Performance improvement.
#define a2_closure_ir(cls, idx)			( assert((idx)<(cls)->len), (cls)->ir_chain[idx] )

#define a2_closure_arg(cls, idx) 		( assert((idx)>=0 && (idx)<(cls)->arg.size), &((cls)->arg.arg_p[idx]) )
#define a2_closure_const(cls, idx)		( assert((idx)<0 && (-1-(idx))<(cls)->c_stack.top), &((cls)->c_stack.stk_p[-1-(idx)]))
#define a2_closure_upvalue(cls, idx)	( assert((idx)>=0 && (idx)<(cls)->upvalue.len), \
											a2_closure_arg((cls)->upvalue.upvalue_chain[idx].cls_p, \
											(cls)->upvalue.upvalue_chain[idx].arg_idx) )
#define a2_closure_container(cls, idx)	( assert((idx)>=0 && (idx)<(cls)->ctn_stack.top), &((cls)->ctn_stack.stk_p[idx]))
#define a2_closure_cls(cls, idx)		( assert((idx)>=0 && (idx)<(cls)->cls_stack.top), &((cls)->cls_stack.stk_p[idx]))

#endif
