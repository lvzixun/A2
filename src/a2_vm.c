#include "a2_conf.h"
#include "a2_ir.h"
#include "a2_closure.h"
#include "a2_env.h"
#include "a2_error.h"
#include "a2_obj.h"
#include "a2_array.h"
#include "a2_map.h"
#include "a2_string.h"

#include <string.h>
#include <stdio.h>

#define META_GETVALUE_COUNT 100

#define ci_ir(ci)	 a2_closure_ir((ci)->cls, (ci)->pc)
#define ci_op(ci)  (ir_gop(ci_ir(ci)))

#define curr_ci    (vm_p->call_chain)
#define curr_pc    (curr_ci->pc)
#define curr_cls   (curr_ci->cls)
#define curr_ir    a2_closure_ir(curr_cls, curr_pc)
#define curr_op    (ir_gop(curr_ir))

#define ci_irdes2sfi(ci)	( (ci)->reg_stack.sf_idx+ir_ga(ci_ir(ci))+ir_gb(ci_ir(ci)) )
// #define curr_irdes2sfi()	(curr_ci->reg_stack.sf_idx+ir_ga(curr_ir)+ir_gb(curr_ir))
#define curr_line  a2_closure_line(curr_cls, curr_pc)

#define sf_reg(sf_idx)					(assert((sf_idx)<vm_p->stack_frame.cap), &(vm_p->stack_frame.sf_p[sf_idx]))

#define callinfo_sfi(ci, idx)		(assert((ci) && (idx)<(ci)->reg_stack.len), (ci)->reg_stack.sf_idx+(idx))
#define callinfo_reg_top(ci)		( (ci)->reg_stack.top_idx )				
#define callinfo_sfreg(ci, idx)	( assert((idx)<(ci)->reg_stack.len), \
																	callinfo_reg_top(ci) = (callinfo_reg_top(ci)<(idx))?(idx):(callinfo_reg_top(ci)), \
																	&(vm_p->stack_frame.sf_p[callinfo_sfi(ci, idx)]))

#define ci_iscls(ci)	((ci)->cls!=NULL)

#define DEF_STACK_FRAME_SIZE	64

#define vm_error(s)  do{a2_error(vm_p->env_p, e_vm_error, \
					"[vm error@%zd]: %s\n", curr_line, s);}while(0)

struct a2_mobj{
	int tag;
	struct a2_obj* obj;
};

struct vm_callinfo{
	struct a2_closure* cls;
	struct {
		size_t sf_idx;
		size_t top_idx;
		size_t len;
	}reg_stack;

	int retbegin;
	int retnumber;
	size_t pc;

	struct vm_callinfo* next; 
	struct vm_callinfo* front;
};

struct  a2_vm{
	struct a2_env* env_p;
	struct {
		struct a2_obj* sf_p;
		size_t cap;
		size_t size;
	}stack_frame;
	struct vm_callinfo* call_chain;
	struct vm_callinfo* call_tail;
	struct vm_callinfo* call_head;
};


static inline int a2_vm_run(struct a2_vm* vm_p);
static inline void _vm_load(struct a2_vm* vm_p);
static inline void _vm_loadnil(struct a2_vm* vm_p);
static inline void _vm_getglobal(struct a2_vm* vm_p);
static inline void _vm_setglobal(struct a2_vm* vm_p);
static inline void _vm_getvalue(struct a2_vm* vm_p);
static inline void _vm_setlist(struct a2_vm* vm_p);
static inline void _vm_setmap(struct a2_vm* vm_p);
static inline void _vm_setvalue(struct a2_vm* vm_p);
static inline void _vm_closure(struct a2_vm* vm_p);
static inline void _vm_forprep(struct a2_vm* vm_p);
static inline void _vm_forloop(struct a2_vm* vm_p);
static inline void _vm_jump(struct a2_vm* vm_p);
static inline void _vm_move(struct a2_vm* vm_p);
static inline void _vm_test(struct a2_vm* vm_p);
static inline void _vm_not(struct a2_vm* vm_p);
static inline void _vm_neg(struct a2_vm* vm_p);
static inline void _vm_get_upvalue(struct a2_vm* vm_p);
static inline void _vm_set_upvalue(struct a2_vm* vm_p);
static inline void _vm_call(struct a2_vm* vm_p);
static inline void __vm_call_function(struct a2_vm* vm_p, struct a2_obj* _func);
static inline void __vm_call_cfunction(struct a2_vm* vm_p, struct a2_obj* _func);
static inline void _vm_foreachloop(struct a2_vm* vm_p);
static inline void _vm_foreachprep(struct a2_vm* vm_p);
static inline void _vm_cat(struct a2_vm* vm_p);
static inline void _vm_newlist(struct a2_vm* vm_p);
static inline void _vm_newmap(struct a2_vm* vm_p);

static inline int _vm_return(struct a2_vm* vm_p, int* ret);
static inline int __vm_return_function(struct a2_vm* vm_p);
static inline int __vm_return_cfunction(struct a2_vm* vm_p);

// meta method
static struct a2_obj* _mgetvalue(struct a2_vm* vm_p, struct a2_obj* c, struct a2_obj* k, int is_raw);

struct a2_vm* a2_vm_new(struct a2_env* env_p){
	struct a2_vm* vm_p = (struct a2_vm*)malloc(sizeof(*vm_p));
	vm_p->env_p = env_p;
	vm_p->call_chain = NULL;
	vm_p->call_tail = NULL;
	vm_p->call_head = NULL;

	vm_p->stack_frame.sf_p = (struct a2_obj*)malloc(sizeof(struct a2_obj)*DEF_STACK_FRAME_SIZE);
	vm_p->stack_frame.cap = 0;
	vm_p->stack_frame.size = DEF_STACK_FRAME_SIZE;
	return vm_p;
}


void a2_vm_free(struct a2_vm* vm_p){
	while(vm_p->call_tail){
		struct vm_callinfo* _p = vm_p->call_tail;
		vm_p->call_tail = _p->next;
		free(_p);
	}
	free(vm_p->stack_frame.sf_p);
	free(vm_p);
}


inline size_t vm_callinfo_sfi(struct vm_callinfo* ci, size_t reg_idx){
	return callinfo_sfi(ci, reg_idx);
}

inline size_t vm_callinfo_regi(struct vm_callinfo* ci, size_t sf_idx){
	assert(sf_idx >= ci->reg_stack.sf_idx && \
		sf_idx <(ci->reg_stack.len + ci->reg_stack.sf_idx));
	return sf_idx - ci->reg_stack.sf_idx;
}


inline  struct vm_callinfo* vm_curr_callinfo(struct a2_vm* vm_p){
	return curr_ci;
}

inline struct a2_closure* vm_callinfo_cls(struct vm_callinfo* ci){
	assert(ci);
	return ci->cls;
}

inline struct a2_obj* vm_sf_index(struct a2_vm* vm_p, size_t sf_idx){
	assert(vm_p);
	assert(sf_idx < vm_p->stack_frame.cap);
	return &(vm_p->stack_frame.sf_p[sf_idx]);
}


// get slice from stack_frame
// the size is max  for move stack.

static inline size_t up_stack_frame(struct a2_vm* vm_p, int size){
	if(vm_p->stack_frame.cap+size > vm_p->stack_frame.size){
		do{
			vm_p->stack_frame.size *=2;
		}while(vm_p->stack_frame.size-vm_p->stack_frame.cap<size);
		vm_p->stack_frame.sf_p = (struct a2_obj*)realloc(vm_p->stack_frame.sf_p,
			vm_p->stack_frame.size*sizeof(struct a2_obj));
	}

	vm_p->stack_frame.cap+=size;
	if(curr_ci==NULL)
		return 0;

	struct vm_callinfo* cip = curr_ci;
	while(cip->cls == NULL){
		cip = cip->next;
		if(cip==NULL)
			return 0;
	}
	assert(ci_op(cip) == CALL);
	size_t ret = ci_irdes2sfi(cip)+1;
	return ret;
}

static inline void down_stack_frame(struct a2_vm* vm_p, int size){
	assert(vm_p->stack_frame.cap - size >=0);
	vm_p->stack_frame.cap -= size;
}

static inline void callinfo_new(struct a2_vm* vm_p, struct a2_closure* cls, int retbegin, int retnumber){
	assert(vm_p);
	struct vm_callinfo* ci = NULL;
	if(vm_p->call_chain == NULL && vm_p->call_head){
		ci = vm_p->call_head;
	}else if(vm_p->call_chain && vm_p->call_chain->front){
		ci = vm_p->call_chain->front;
	}else{
		assert(vm_p->call_chain == vm_p->call_tail);
		ci = (struct vm_callinfo*)malloc(sizeof(*ci));
		if(vm_p->call_head == NULL) vm_p->call_head = ci;  // set call info head 
		vm_p->call_tail = ci;
		ci->next = vm_p->call_chain;
		if(vm_p->call_chain)
			vm_p->call_chain->front = ci;
		ci->front = NULL;
	}

	ci->cls = cls;
	// set closure stack frame
	ci->reg_stack.top_idx = 0;
	ci->reg_stack.len = (cls)?(a2_closure_regscount(cls)):(0);
	ci->reg_stack.sf_idx = up_stack_frame(vm_p, ci->reg_stack.len);
	ci->pc=0;
	ci->retbegin = retbegin;
	ci->retnumber = retnumber;
	vm_p->call_chain = ci;
}

static inline void callinfo_free(struct a2_vm* vm_p){
	assert(curr_ci);
	struct vm_callinfo* b = curr_ci->next;
	down_stack_frame(vm_p, curr_ci->reg_stack.len);
	curr_ci = b;
}


static void _vm_run(struct a2_env* env_p, struct a2_vm* vm_p){
	a2_vm_run(vm_p);
}

static int _vm_prun(struct a2_vm* vm_p){
	struct vm_callinfo* b_ci_p = curr_ci;

	int ret = a2_xpcall(vm_p->env_p, (a2_pfunc)_vm_run, vm_p);
	if(ret){
		struct vm_callinfo* p = curr_ci;
		while(p!=b_ci_p->next){
			callinfo_free(vm_p);
			p = p->next;
		}
	}
	return ret;
}

int a2_vm_pcall(struct a2_vm* vm_p, struct a2_obj* cls_obj, struct a2_obj* args_obj, int args){
	assert(obj_t(cls_obj) == A2_TCLOSURE);
	struct a2_closure* cls = a2_gcobj2closure(obj_vX(cls_obj, obj));
	callinfo_new(vm_p, NULL, 0, 0);
	callinfo_new(vm_p, cls, 0, 0);

	// set arg
	int i, j;
	for(i=0, j=0; 
		i<args && j<curr_ci->reg_stack.len;
		j++, i++){
		*callinfo_sfreg(curr_ci, j) = args_obj[i];
	}

	// set clear
	for(; j<curr_ci->reg_stack.len; j++){
		*callinfo_sfreg(curr_ci, j) = a2_nil2obj();
	}
	int ret = _vm_prun(vm_p);
	callinfo_free(vm_p);

	return ret;
}

int a2_vm_load(struct a2_vm* vm_p, struct a2_closure* cls){
	assert(vm_p && cls);
	callinfo_new(vm_p, cls, 0, 0);
	return _vm_prun(vm_p);
}


#define _getvalue(vm_p, idx)	( ((idx)<0)?(a2_closure_const((curr_cls), (idx))):(callinfo_sfreg((curr_ci), (idx))) )


#define _vm_op(op)	 		__vm_op(op, a2_number2obj)
#define _vm_oplimit(op) 	__vm_op(op, a2_bool2obj)

#define  __vm_op(op, f) do{struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir)); \
							struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
							struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
							if(obj_t(_v1)!=A2_TNUMBER || obj_t(_v2)!=A2_TNUMBER) \
								vm_error("the varable is not number."); \
							*_d = f(obj_vNum(_v1) op obj_vNum(_v2));\
							curr_pc++;}while(0)

#define _vm_ope(op)		do{struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir)); \
							struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
							struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
							if(obj_t(_v1) != obj_t(_v2)) \
								vm_error("the varables is different."); \
							switch(obj_t(_v1)){ \
								case A2_TNUMBER: \
									*_d = a2_bool2obj(obj_vNum(_v1) op obj_vNum(_v2)); \
									break; \
								case A2_TSTRING: \
									*_d = a2_bool2obj(a2_gcobj2string(obj_vX(_v1, obj)) op a2_gcobj2string(obj_vX(_v2, obj))); \
									break; \
								case A2_TNIL: \
									*_d = a2_bool2obj(1); \
									break; \
								case A2_TARRAY: \
								case A2_TCLOSURE: \
								case A2_TMAP: \
									*_d = a2_bool2obj(obj_vX(_v1, obj) op obj_vX(_v2, obj)); \
									break; \
							} \
							curr_pc++;}while(0)


#define _vm_opl(op)  do{struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir)); \
						struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
						struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
						if(obj_t(_v1)!=A2_TBOOL || obj_t(_v2)!=A2_TBOOL) \
							vm_error("the varable is must bool."); \
						*_d = a2_bool2obj( obj_vX(_v1, uinteger) op obj_vX(_v2, uinteger)); \
						curr_pc++;}while(0)


#define jump(idx)	do{struct a2_obj* _oa = a2_closure_const(curr_cls, (idx)); \
						assert(obj_t(_oa) == _A2_TADDR); \
						curr_pc = obj_vX(_oa, addr);}while(0)

#ifdef A2_JIT
// a2 jit
#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"
#include "a2_jit.h"

// jit bytecodes
static int a2_vm_run(struct a2_vm* vm_p){
	// if not jit build, so build it
	if(!xcls_is_jit(curr_cls->xcls_p)){
		curr_cls->xcls_p->jit_func = _jit_build(vm_p);
	}

	int ret =  xcls_call_jit(curr_cls->xcls_p);
	#ifdef _DEBUG_
	printf("call jit success!\n");
	#endif
	return ret;
}

#else 

 // Interpreter bytecodes
static int a2_vm_run(struct a2_vm* vm_p){
	int ret = 0;

	for(;;){
		switch(curr_op){
			case LOAD:
				_vm_load(vm_p);
				break;
			case LOADNIL:
				_vm_loadnil(vm_p);
				break;
			case GETGLOBAL:
				_vm_getglobal(vm_p);
				break;
			case SETGLOBAL:
				_vm_setglobal(vm_p);
				break;
			case GETUPVALUE:
				_vm_get_upvalue(vm_p);
				break;
			case SETUPVALUE:
				_vm_set_upvalue(vm_p);
				break;
			case NEWLIST:
				_vm_newlist(vm_p);
				break;
			case NEWMAP:
				_vm_newmap(vm_p);
				break;
			case GETVALUE:
				_vm_getvalue(vm_p);
				break;
			case SETVALUE:
				_vm_setvalue(vm_p);
				break;
			case SETLIST:
				_vm_setlist(vm_p);
				break;
			case SETMAP:
				_vm_setmap(vm_p);
				break;
			case CLOSURE:
				_vm_closure(vm_p);
				break;
			case FOREACHLOOP:
				_vm_foreachloop(vm_p);
				break;
			case FOREACHPREP:
				_vm_foreachprep(vm_p);
				break;
			case FORPREP:
				_vm_forprep(vm_p);
				break;
			case FORLOOP:
				_vm_forloop(vm_p);
				break;
			case JUMP:
				_vm_jump(vm_p);
				break;
			case MOVE:
				_vm_move(vm_p);
				break;
			case TEST:
				_vm_test(vm_p);
				break;
			case ADD:
				_vm_op(+);
				break;
			case SUB:
				_vm_op(-);
				break;
			case DIV:{
				struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir));
				if(obj_t(_v2)==A2_TNUMBER && obj_vNum(_v2)==0)
					vm_error("division 0.");
				 _vm_op(/);
				}break;
			case MUL:
				_vm_op(*);
				break;
			case OR:
				_vm_opl(||);
				break;
			case AND:
				_vm_opl(&&);
				break;
			case BIG:
				_vm_oplimit(>);
				break;
			case LITE:
				_vm_oplimit(<);
				break;
			case BIGE:
				_vm_oplimit(>=);
				break;
			case LITEE:
				_vm_oplimit(<=);
				break;
			case EQU:
				_vm_ope(==);
				break;
			case NEQU:
				_vm_ope(!=);
				break;
			case NOT:
				_vm_not(vm_p);
				break;
			case NEG:
				_vm_neg(vm_p);
				break;
			case CAT:
				_vm_cat(vm_p);
				break;
			case CALL:
				_vm_call(vm_p);
				break;
			case RETURN:
				if(_vm_return(vm_p, &ret))
					return ret;
				break;
			default:
				assert(0);
		}
	}
	return ret;
}
#endif

// load
static inline void _vm_load(struct a2_vm* vm_p){
	struct a2_obj* _des_obj = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	*_des_obj = *(a2_closure_const(curr_cls, ir_gbx(curr_ir)));
	curr_pc++;
}

// loadnil
static inline void _vm_loadnil(struct a2_vm* vm_p){
	int i;
	struct a2_obj* _obj = NULL;
	for(i=ir_ga(curr_ir); i<(ir_ga(curr_ir)+ir_gbx(curr_ir)); i++){
		_obj = callinfo_sfreg(curr_ci, i);
		obj_setX(_obj, A2_TNIL, point, NULL);
	}
	curr_pc++;
}

//get global varable
static inline void _vm_getglobal(struct a2_vm* vm_p){
	int bx = ir_gbx(curr_ir);
	struct a2_obj* _dobj = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, bx);
	struct a2_obj* _obj = a2_get_envglobal(vm_p->env_p, _k);

	if(_obj==NULL){
		char _buf[64] = {0};
		a2_error(vm_p->env_p, e_vm_error, 
			"[vm error@%zd]: the global \'%s\' is not find.\n", 
			curr_line, 
			obj2str(_k, _buf, sizeof(_buf)));
	}
	*_dobj = *_obj;
	curr_pc++;
}


// set global varable
static inline void _vm_setglobal(struct a2_vm* vm_p){
	struct a2_obj* _k = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	
	*_d = *a2_set_envglobal(vm_p->env_p, _k, _v);
	curr_pc++;	
}

// new list
static inline void _vm_newlist(struct a2_vm* vm_p){
	struct a2_gcobj* _gcobj = a2_array2gcobj(a2_array_new());
	a2_gcadd(vm_p->env_p, _gcobj);

	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	obj_setX(_d, A2_TARRAY, obj,  _gcobj);
	curr_pc++;
}

// new map
static inline void _vm_newmap(struct a2_vm* vm_p){
	struct a2_gcobj* _gcobj = a2_map2gcobj(a2_map_new());
	a2_gcadd(vm_p->env_p, _gcobj);
	
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	obj_setX(_d, A2_TMAP, obj, _gcobj);
	curr_pc++;
}

//set list
static inline void _vm_setlist(struct a2_vm* vm_p){
	int i, end=ir_gb(curr_ir)+ir_gc(curr_ir);
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));	
	assert(obj_t(_d)==A2_TARRAY);
	for(i=ir_gb(curr_ir); i<end; i++){
		a2_array_add(a2_gcobj2array(obj_vX(_d, obj)), callinfo_sfreg(curr_ci, i));
	}
	curr_pc++;
}

//set map
static inline void _vm_setmap(struct a2_vm* vm_p){
	int i, end=ir_gb(curr_ir)+2*ir_gc(curr_ir);
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = NULL;
	struct a2_map* map = NULL;
	assert(obj_t(_d)==A2_TMAP);
	
	struct a2_kv kv={0};
	for(i=ir_gb(curr_ir); i<end; i+=2){
		kv.key = callinfo_sfreg(curr_ci, i);
		kv.vp = callinfo_sfreg(curr_ci, i+1);
		map = a2_gcobj2map(obj_vX(_d, obj));
		if( (_v=a2_map_query(map, kv.key))==NULL )
			a2_map_add(map, &kv);
		else
			*_v = *kv.vp;
	}
	curr_pc++;
}

static inline struct a2_obj* _vm_meta_getvalue(struct a2_vm* vm_p, struct a2_obj* c, struct a2_obj* k){
	return _mgetvalue(vm_p, c, k, 0);
}


static struct a2_obj* _mgetvalue(struct a2_vm* vm_p, struct a2_obj* c, struct a2_obj* k, int is_raw){
	assert(obj_t(c) == A2_TMAP);
	assert(obj_t(k)==A2_TNUMBER || obj_t(k)==A2_TSTRING);

	int i;
	for(i=0; i<META_GETVALUE_COUNT; i++){
		struct a2_map* map = a2_gcobj2map(obj_vX(c, obj));
		struct a2_obj* v = a2_map_query(map, k);
		if(v == NULL && is_raw)
			goto GET_ERROR;
		else if(v)
			return v;
		else if(!is_raw){
			struct a2_gcobj*  meta = obj_vX(c, obj)->meta;
			if(!meta)
				goto GET_ERROR;
			else{
				assert(meta->type==A2_TMAP);
				assert(!is_raw==1);
				
				struct a2_obj* _mk_index = a2_env_getmk(vm_p->env_p, MK_INDEX);
				struct a2_obj* _m_obj = a2_map_query(a2_gcobj2map(meta), _mk_index);
				if(!_m_obj) goto GET_ERROR;

				int type = obj_t(_m_obj);
				if(type==A2_TMAP){
					c = _m_obj;
					continue;
				}else if(type==A2_TCLOSURE){
					vm_error("__index is closure will add in the future.");
				}else{
					assert(0);
				}
			}
		}else
			goto GET_ERROR;
	}
	vm_error("loop get meta value.");

GET_ERROR:
 	vm_error("the key is overfllow.");

	return NULL;
}

// get value 
static inline void _vm_getvalue(struct a2_vm* vm_p){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_gb(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(obj_t(_c)){
		case A2_TARRAY:
	 		if(obj_t(_k)!=A2_TNUMBER)
	 			vm_error("the key is must number at get array.");
			__d = a2_array_get(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(!__d) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		case A2_TMAP:
			if(obj_t(_k)!=A2_TNUMBER && obj_t(_k)!=A2_TSTRING)
				vm_error("the key is must number or string at get map.");
			__d = _vm_meta_getvalue(vm_p, _c, _k);
			// __d = a2_map_query(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(!__d) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		default:
			vm_error("the varable is not map or array.");
	}

 	curr_pc++;
 	return;
GVALUE_ERROR:
 	vm_error("the key is overfllow.");
}

// set value
static inline void _vm_setvalue(struct a2_vm* vm_p){
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(obj_t(_c)){
		case A2_TARRAY:
	 		if(obj_t(_k)!=A2_TNUMBER)
	 			vm_error("the key is must number at set array.");
			__d = a2_array_get(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(!__d)  goto SVALUE_ERROR;
			*__d = *_v;
			break;
		case A2_TMAP:
			if(obj_t(_k)!=A2_TNUMBER && obj_t(_k)!=A2_TSTRING)
				vm_error("the key is must number or string at set map.");
			__d = a2_map_query(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(!__d) goto SVALUE_ERROR;
			*__d = *_v;
			break;
		default:
			vm_error("the varable is not map or array.");
	}

 	curr_pc++;
 	return;
 SVALUE_ERROR:
 	vm_error("the key is overfllow.");
}

// closure
static inline void _vm_closure(struct a2_vm* vm_p){
	struct a2_closure* _cls = a2_closure_new(curr_ci, ir_gbx(curr_ir));
	struct a2_gcobj* _gcobj = a2_closure2gcobj(_cls);
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	obj_setX(_d, A2_TCLOSURE, obj, _gcobj);
	a2_gcadd(vm_p->env_p, _gcobj);
	curr_pc++;
}

// forprep
static inline void _vm_forprep(struct a2_vm* vm_p){
	struct a2_obj* _i = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _count = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _step = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);

	if(obj_t(_i)!=A2_TNUMBER)
		vm_error("the index varable is must number.");
	if(obj_t(_count)!=A2_TNUMBER)
		vm_error("the for's count is must number.");
	if(obj_t(_step)!=A2_TNUMBER)
		vm_error("the for's step is must number.");

	if(obj_vNum(_i)>=obj_vNum(_count))  // for end
		jump(ir_gbx(curr_ir));
	else	// for continue
		curr_pc++;
}

// forloop
static inline void _vm_forloop(struct a2_vm* vm_p){
	struct a2_obj* _i = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _count = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _step = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);

	if(obj_t(_i)!=A2_TNUMBER)
		vm_error("the index varable is must number.");

	assert(obj_t(_i)==A2_TNUMBER && obj_t(_count)==A2_TNUMBER && obj_t(_step)==A2_TNUMBER);
	obj_vNum(_i) += obj_vNum(_step);
	if(obj_vNum(_i) < obj_vNum(_count)) // continue for
		jump(ir_gbx(curr_ir));
	else // for end
		curr_pc++;
}

// foreachprep 
static inline void _vm_foreachprep(struct a2_vm* vm_p){
	struct a2_obj* _k = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(obj_t(_c)!=A2_TMAP && obj_t(_c)!=A2_TARRAY)
		vm_error("the varable is not map or array.");

	// set init varable
	switch(obj_t(_c)){
		case A2_TMAP:
			*_k = a2_nil2obj();
			__v = a2_map_next(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(__v==NULL)
				jump(ir_gbx(curr_ir));
			else{
				*_v = *__v;
				curr_pc++;
			}
			break;
		case A2_TARRAY:
			*_k = a2_nil2obj();
			__v = a2_array_next(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(__v==NULL)  // dump is end
				jump(ir_gbx(curr_ir));
			else{
				*_v = *__v;
				curr_pc++;
			}
			break;
		default:
			assert(0);
	}
}

// foreachloop
static inline void _vm_foreachloop(struct a2_vm* vm_p){
	struct a2_obj* _k = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(obj_t(_c)!=A2_TMAP && obj_t(_c)!=A2_TARRAY)
		vm_error("the varable is not map or array.");

	// dump next
	switch(obj_t(_c)){
		case A2_TMAP:
			__v = a2_map_next(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(__v==NULL)
				curr_pc++;
			else{
				*_v = *__v;
				jump(ir_gbx(curr_ir));
			}
			break;
		case A2_TARRAY:
			__v = a2_array_next(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(__v==NULL)  // dump is end
				curr_pc++;
			else{
				*_v = *__v;
				jump(ir_gbx(curr_ir));
			}
			break;
		default:
			assert(0);
	}
}

// jump
static inline void _vm_jump(struct a2_vm* vm_p){
	jump(ir_gbx(curr_ir));
}

// move
static inline void _vm_move(struct a2_vm* vm_p){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	*_d = *callinfo_sfreg(curr_ci, ir_gbx(curr_ir));
	curr_pc++;
}

// test
static inline void _vm_test(struct a2_vm* vm_p){
	struct  a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	if(obj_t(_v)==A2_TNIL || (obj_t(_v)==A2_TBOOL && !(obj_vX(_v, uinteger))))
		jump(ir_gbx(curr_ir));
	else
		curr_pc++;
}

// not 
static inline void _vm_not(struct a2_vm* vm_p){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	switch(obj_t(_v)){
		case A2_TNIL:
			*_d = a2_bool2obj(1);
			break;
		case A2_TBOOL:
			*_d = a2_bool2obj(!(obj_vX(_v, uinteger)));
			break;
		default:
			vm_error("the varable is not bool type.");
	}

	curr_pc++;
}

// neg
static inline void _vm_neg(struct a2_vm* vm_p){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	if(obj_t(_v)!=A2_TNUMBER)
		vm_error("the varable is not number type at set neg.");
	*_d = a2_number2obj(obj_vNum(_v)*(-1));
	curr_pc++;
}

// getupvalue
static inline void _vm_get_upvalue(struct a2_vm* vm_p){
	struct a2_upvalue* _uv = a2_closure_upvalue(curr_cls, ir_gbx(curr_ir));
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));

	switch(_uv->type){
		case uv_stack:
			*_d = *sf_reg(_uv->v.sf_idx);
			break;
		case uv_gc:
			*_d = *a2_gcobj2upvalue(_uv->v.uv_obj);
			break;
		default:
			assert(0);
	}
	curr_pc++;
}

// set upvalue
static inline void _vm_set_upvalue(struct a2_vm* vm_p){
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));
	struct a2_upvalue* _uv_d = a2_closure_upvalue(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _ud = NULL;

	switch(_uv_d->type){
		case uv_stack:
			*sf_reg(_uv_d->v.sf_idx) = *_v;
			break;
		case uv_gc:
			_ud = a2_gcobj2upvalue(_uv_d->v.uv_obj);
			*_ud = *_v;
			break;
		default:
			assert(0);
	}
	curr_pc++;
}

// cat
static inline void _vm_cat(struct a2_vm* vm_p){
	struct a2_obj* _lv = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _rv = _getvalue(vm_p, ir_gc(curr_ir));

	char buf[64] = {0};
	char buf0[64] = {0};
	char* a2_s = a2_string_new(obj2str(_lv, buf, sizeof(buf)-1));
	a2_s = a2_string_cat(a2_s, obj2str(_rv, buf0, sizeof(buf0)-1));

	struct a2_gcobj* gcobj = a2_env_addstrobj(vm_p->env_p, a2_s);
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	obj_setX(_d, A2_TSTRING, obj, gcobj);

	a2_string_free(a2_s);
	curr_pc++;
}

// call
static inline void _vm_call(struct a2_vm* vm_p){
	struct a2_obj* _func = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	switch(obj_t(_func)){
		case A2_TCLOSURE:
			__vm_call_function(vm_p, _func);
			break;
		case A2_TCFUNCTION:
			__vm_call_cfunction(vm_p, _func);
			break;
		default:
			vm_error("the varable is not function type.");
	}
}

// call c function
static inline void __vm_call_cfunction(struct a2_vm* vm_p, struct a2_obj* _func){
	assert(obj_t(_func)==A2_TCFUNCTION);
	int i, j;
	struct a2_obj* _obj = NULL;

	// back bottom
	int _b = a2_getbottom(vm_p->env_p);
	int _top = a2_gettop(vm_p->env_p);

	a2_setbottom(vm_p->env_p, a2_gettop(vm_p->env_p));

	// closure arg to cstack
	for(i=ir_ga(curr_ir)+1; i<=ir_ga(curr_ir)+ir_gb(curr_ir); i++){
		_obj = callinfo_sfreg(curr_ci, i);
		a2_pushstack(vm_p->env_p, _obj);
	}

	// call c function
	callinfo_new(vm_p, NULL, 0, 0);
	int ret = obj_vX(_func, cfunction)(a2_env2state(vm_p->env_p));
	ret = (ret<0)?(0):(ret);
	callinfo_free(vm_p);

	int size = a2_gettop(vm_p->env_p)-a2_getbottom(vm_p->env_p);
	// set return value
	for(i=size-ret, j=ir_ga(curr_ir); 
		i<size && j<ir_ga(curr_ir)+ir_gc(curr_ir);
		j++,i++){
		_obj = callinfo_sfreg(curr_ci, j);
		*_obj = *a2_getcstack(vm_p->env_p, i);
	}

	for(; j<ir_ga(curr_ir)+ir_gc(curr_ir); j++){
		_obj = callinfo_sfreg(curr_ci, j);
		obj_setX(_obj, A2_TNIL, point, NULL);
	}

	a2_setbottom(vm_p->env_p, _b);
	a2_settop(vm_p->env_p, _top);
	curr_pc++;
}

// call a2 function
static inline void __vm_call_function(struct a2_vm* vm_p, struct a2_obj* _func){
	assert(obj_t(_func)==A2_TCLOSURE);
	struct a2_obj* _obj = NULL;
	int i, j, params = a2_closure_params(a2_gcobj2closure(obj_vX(_func, obj)));
	struct a2_array* _args = NULL;
	struct vm_callinfo* _ci = curr_ci;
	ir _ir = curr_ir;

	// new call info
	int b = ir_ga(curr_ir), n=ir_gc(curr_ir);
	callinfo_new(vm_p, a2_gcobj2closure(obj_vX(_func, obj)), b, n);
	// jump call
	_ci->pc++;

	// if is mutableargs
	if(params<0){ 
		params = -1 - params;
		_obj = callinfo_sfreg(curr_ci, params);
		if(ir_gb(_ir)>params){	// set _args list
			_args = a2_array_new();
			struct a2_gcobj* _array_gcobj = a2_array2gcobj(_args);
			a2_gcadd(vm_p->env_p, _array_gcobj);
			obj_setX(_obj, A2_TARRAY, obj, _array_gcobj);
		}else{
			obj_setX(_obj, A2_TNIL, point, NULL);
		}
	}

	// set params
	for(i=ir_ga(_ir)+1, j=0; 
		i<=ir_ga(_ir)+ir_gb(_ir) && j<params; 
		j++, i++){
		_obj = callinfo_sfreg(curr_ci, j);
		*_obj = *callinfo_sfreg(_ci, i);
	}
	
	// set clear params
	for( ;j<params; j++){
		_obj = callinfo_sfreg(curr_ci, j);
		obj_setX(_obj, A2_TNIL, point, NULL);
	}

	// if mutable args
	for(j=0; i<=ir_ga(_ir)+ir_gb(_ir) && _args; i++, j++){
		a2_array_add(_args, callinfo_sfreg(_ci, i));
	}
}

// return
static inline int _vm_return(struct a2_vm* vm_p, int* ret){
	// the closure is end
	if(curr_ci->next==NULL){
		callinfo_free(vm_p);
		*ret = 0;
		return 1;
	}

	// if return to a2 function
	if(curr_ci->next->cls){
		*ret = __vm_return_function(vm_p);
		return 0;
	}else{
		*ret = __vm_return_cfunction(vm_p);
		return 1;
	}
}

// return to c function
static inline int __vm_return_cfunction(struct a2_vm* vm_p){
	struct a2_obj* _obj = NULL;
	int i, ret;

	// return to c stack
	for(i=ir_ga(curr_ir); i<ir_ga(curr_ir)+ir_gbx(curr_ir); i++){
		_obj = callinfo_sfreg(curr_ci, i);
		a2_pushstack(vm_p->env_p, _obj);
	}
	ret = ir_gbx(curr_ir);
	a2_closure_return(curr_cls, vm_p->env_p);
	callinfo_free(vm_p);
	return ret;
}

// return to a2 function
static inline int __vm_return_function(struct a2_vm* vm_p){
	struct a2_obj* _obj = NULL;
	struct vm_callinfo* call_ci = curr_ci->next;
	int i, j, ret;

	// set uped value
	a2_closure_return(curr_cls, vm_p->env_p);

	// set return
	for(i=ir_ga(curr_ir), j=curr_ci->retbegin; 
		i<ir_ga(curr_ir)+ir_gbx(curr_ir) && j<curr_ci->retbegin+curr_ci->retnumber;
		 j++, i++){
		_obj = callinfo_sfreg(call_ci, j);
		*_obj = *callinfo_sfreg(curr_ci, i);
	}

	// set clear
	for( ; j<curr_ci->retbegin+curr_ci->retnumber; j++){
		_obj = callinfo_sfreg(call_ci, j);
		obj_setX(_obj, A2_TNIL, point, NULL);
	}

	ret = curr_ci->retnumber;
	callinfo_free(vm_p);
	return ret;
}

// mark and clear gc
void a2_vm_gc(struct a2_vm* vm_p){
	if(curr_ci==NULL)
		goto STACK_FRAME_END;

	struct vm_callinfo* cip = curr_ci;

	while(!ci_iscls(cip)){
		cip = cip->next;
		if(cip==NULL)
			goto STACK_FRAME_END;
	}

	size_t i, end;
	struct a2_obj* obj = NULL;
	end = cip->reg_stack.sf_idx+cip->reg_stack.top_idx+1;

	assert(end <= vm_p->stack_frame.cap);

	// mark the stack frame
	for(i=0; i<end; i++){
		obj = &(vm_p->stack_frame.sf_p[i]);
		a2_gc_markit(a2_envgc(vm_p->env_p), obj, mark_black);
	}

STACK_FRAME_END:
	#ifdef _DEBUG_
	printf("\n---gc mark----\n");
	dump_gc(a2_envgc(vm_p->env_p));
	#endif
	// clear
	a2_gc_clear(a2_envgc(vm_p->env_p));
	
	#ifdef _DEBUG_
	printf("----gc clear-----\n");
	dump_gc(a2_envgc(vm_p->env_p));
	#endif
}



