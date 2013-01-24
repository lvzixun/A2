#include "a2_conf.h"
#include "a2_ir.h"
#include "a2_closure.h"
#include "a2_env.h"
#include "a2_error.h"
#include "a2_obj.h"
#include "a2_array.h"
#include "a2_map.h"

#define curr_ci    (vm_p->call_chain)
#define curr_pc    (curr_ci->pc)
#define curr_cls   (curr_ci->cls)
#define curr_ir    a2_closure_ir(curr_cls, curr_pc)
#define curr_op    (ir_gop(curr_ir))
#define curr_line  a2_closure_line(curr_cls, curr_pc)
   
#define vm_error(s)  do{a2_error("[vm error@%lu]: %s\n", curr_line, s);}while(0)


struct vm_callinfo{
	struct a2_closure* cls;
	int retbegin;
	int retnumber;
	size_t pc;

	struct vm_callinfo* next; 
	struct vm_callinfo* front;
};

struct  a2_vm{
	struct a2_env* env_p;

	struct vm_callinfo* call_chain;
};

static inline struct a2_obj* _getvalue(struct a2_vm* vm_p, int idx);

static inline int a2_vm_run(struct a2_vm* vm_p);
static inline void _vm_load(struct a2_vm* vm_p);
static inline void _vm_loadnil(struct a2_vm* vm_p);
static inline void _vm_getglobal(struct a2_vm* vm_p);
static inline void _vm_setglobal(struct a2_vm* vm_p);
static inline void _vm_getvalue(struct a2_vm* vm_p);
static inline void _vm_container(struct a2_vm* vm_p);
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

static inline int _vm_return(struct a2_vm* vm_p, int* ret);
static inline int __vm_return_function(struct a2_vm* vm_p);
static inline int __vm_return_cfunction(struct a2_vm* vm_p);

struct a2_vm* a2_vm_new(struct a2_env* env_p){
	struct a2_vm* vm_p = (struct a2_vm*)malloc(sizeof(*vm_p));
	vm_p->env_p = env_p;
	return vm_p;
}


void a2_vm_free(struct a2_vm* vm_p){
	free(vm_p);
}

static inline void callinfo_new(struct a2_vm* vm_p, struct a2_closure* cls, int retbegin, int retnumber){
	assert(vm_p);
	struct vm_callinfo* ci = (struct vm_callinfo*)malloc(sizeof(*ci));
	ci->cls = cls;
	ci->pc=0;
	ci->front = NULL;
	ci->retbegin = retbegin;
	ci->retnumber = retnumber;
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
	callinfo_new(vm_p, cls, 0, 0);
	a2_vm_run(vm_p);
}

#define _vm_op(op)	 		__vm_op(op, a2_number2obj)
#define _vm_oplimit(op) 	__vm_op(op, a2_bool2obj)

#define  __vm_op(op, f) do{struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir)); \
							struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
							struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
							if(_v1->type!=A2_TNUMBER || _v2->type!=A2_TNUMBER) \
								vm_error("the varable is not number."); \
							*_d = f(_v1->value.number op _v2->value.number);\
							curr_pc++;}while(0)

#define _vm_ope(op)		do{struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir)); \
							struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
							struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
							if(_v1->type != _v2->type) \
								vm_error("the varables is different."); \
							switch(_v1->type){ \
								case A2_TNUMBER: \
									*_d = a2_bool2obj(_v1->value.number op _v2->value.number); \
									break; \
								case A2_TSTRING: \
									*_d = a2_bool2obj(a2_gcobj2string(_v1->value.obj) op a2_gcobj2string(_v2->value.obj)); \
									break; \
								case A2_TNIL: \
									*_d = a2_bool2obj(1); \
									break; \
								case A2_TARRAY: \
								case A2_TCLOSURE: \
								case A2_TMAP: \
									*_d = a2_bool2obj(_v1->value.obj op _v2->value.obj); \
									break; \
							} \
							curr_pc++;}while(0)


#define _vm_opl(op)  do{struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir)); \
						struct a2_obj* _v1 = _getvalue(vm_p, ir_gb(curr_ir)); \
						struct a2_obj* _v2 = _getvalue(vm_p, ir_gc(curr_ir)); \
						if(_v1->type!=A2_TBOOL || _v2->type!=A2_TBOOL) \
							vm_error("the varable is must bool."); \
						*_d = a2_bool2obj( _v1->value.uinteger op _v2->value.uinteger); \
						curr_pc++;}while(0)

// vm 
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
			case CONTAINER:
				_vm_container(vm_p);
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
			case DIV:
				_vm_op(/);
				break;
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

static inline struct a2_obj* _getvalue(struct a2_vm* vm_p, int idx){
	return (idx<0)?(a2_closure_const(curr_cls, idx)):
					(a2_closure_arg(curr_cls, idx));
}

//get global varable
static inline void _vm_getglobal(struct a2_vm* vm_p){
	int bx = ir_gbx(curr_ir);
	struct a2_obj* _dobj = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, bx);
	struct a2_obj* _obj = a2_get_envglobal(vm_p->env_p, _k);

	if(_obj==NULL){
		char _buf[64] = {0};
		a2_error("[vm error@%lu]: the global \'%s\' is not find.\n", 
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

// load container
static inline void _vm_container(struct a2_vm* vm_p){
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	*_d = *a2_closure_container(curr_cls, ir_gbx(curr_ir));
	curr_pc++;
}

//set list
static inline void _vm_setlist(struct a2_vm* vm_p){
	int i, end=ir_gb(curr_ir)+ir_gc(curr_ir);
	struct a2_obj* _d = a2_closure_container(curr_cls, ir_ga(curr_ir));	
	assert(_d->type==A2_TARRAY);
	for(i=ir_gb(curr_ir); i<end; i++){
		a2_array_add(a2_gcobj2array(_d->value.obj), a2_closure_arg(curr_cls, i));
	}
	curr_pc++;
}

//set map
static inline void _vm_setmap(struct a2_vm* vm_p){
	int i, end=ir_gb(curr_ir)+2*ir_gc(curr_ir);
	struct a2_obj* _d = a2_closure_container(curr_cls, ir_ga(curr_ir));
	assert(_d->type==A2_TMAP);
	struct a2_kv kv;
	for(i=ir_gb(curr_ir); i<end; i+=2){
		kv.key = a2_closure_arg(curr_cls, i);
		kv.vp = a2_closure_arg(curr_cls, i+1);
		assert(a2_map_add(a2_gcobj2map(_d->value.obj), &kv)==a2_true);
	}
	curr_pc++;
}

// get value 
static inline void _vm_getvalue(struct a2_vm* vm_p){
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _c = a2_closure_arg(curr_cls, ir_gb(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(_c->type){
		case A2_TARRAY:
			if(_k->type!=A2_TNUMBER)
				vm_error("the key is must number at get array.");
			__d = a2_array_get(a2_gcobj2array(_c->value.obj), _k);
			if(__d==NULL) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		case A2_TMAP:
			if(_k->type!=A2_TNUMBER && _k->type!=A2_TSTRING)
				vm_error("the key is must number or string at get map.");
			__d = a2_map_query(a2_gcobj2map(_c->value.obj), _k);
			if(__d==NULL) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		default:
			vm_error("the varable is not container.");
	}

	curr_pc++;
	return;
GVALUE_ERROR:
	vm_error("the key is overfllow at get array.");
}

// set value
static inline void _vm_setvalue(struct a2_vm* vm_p){
	struct a2_obj* _c = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(_c->type){
		case A2_TARRAY:
			if(_k->type!=A2_TNUMBER)
				vm_error("the key is must number at set array.");
			__d = a2_array_get(a2_gcobj2array(_c->value.obj), _k);
			if(__d==NULL)  goto SVALUE_ERROR;
			*__d = *_v;
			break;
		case A2_TMAP:
			if(_k->type!=A2_TNUMBER && _k->type!=A2_TSTRING)
				vm_error("the key is must number or string at set map.");
			__d = a2_map_query(a2_gcobj2map(_c->value.obj), _k);
			if(__d==NULL) goto SVALUE_ERROR;
			*__d = *_v;
			break;
		default:
			vm_error("the varable is not container.");
	}

	curr_pc++;
	return;
SVALUE_ERROR:
	vm_error("the key is overfllow at set array.");
}

#define jump(idx)	do{struct a2_obj* _oa = a2_closure_const(curr_cls, (idx)); \
						assert(_oa->type = _A2_TADDR); \
						curr_pc = _oa->value.addr;}while(0)

// closure
static inline void _vm_closure(struct a2_vm* vm_p){
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	*_d = *a2_closure_cls(curr_cls, ir_gbx(curr_ir));
	curr_pc++;
}

// forprep
static inline void _vm_forprep(struct a2_vm* vm_p){
	struct a2_obj* _i = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _count = a2_closure_arg(curr_cls, ir_ga(curr_ir)+1);
	struct a2_obj* _step = a2_closure_arg(curr_cls, ir_ga(curr_ir)+2);

	if(_i->type!=A2_TNUMBER)
		vm_error("the index varable is must number.");
	if(_count->type!=A2_TNUMBER)
		vm_error("the for's count is must number.");
	if(_step->type!=A2_TNUMBER)
		vm_error("the for's step is must number.");

	if(_i->value.number>=_count->value.number)  // for end
		jump(ir_gbx(curr_ir));
	else	// for continue
		curr_pc++;
}

// forloop
static inline void _vm_forloop(struct a2_vm* vm_p){
	struct a2_obj* _i = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _count = a2_closure_arg(curr_cls, ir_ga(curr_ir)+1);
	struct a2_obj* _step = a2_closure_arg(curr_cls, ir_ga(curr_ir)+2);

	if(_i->type!=A2_TNUMBER)
		vm_error("the index varable is must number.");

	assert(_i->type==A2_TNUMBER && _count->type==A2_TNUMBER && _step->type==A2_TNUMBER);
	_i->value.number += _step->value.number;
	if(_i->value.number<_count->value.number) // continue for
		jump(ir_gbx(curr_ir));
	else // for end
		curr_pc++;
}

// foreachprep 
static inline void _vm_foreachprep(struct a2_vm* vm_p){
	struct a2_obj* _k = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _v = a2_closure_arg(curr_cls, ir_ga(curr_ir)+1);
	struct a2_obj* _c = a2_closure_arg(curr_cls, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(_c->type!=A2_TMAP && _c->type!=A2_TARRAY)
		vm_error("the varable is not container.");

	// set init varable
	switch(_c->type){
		case A2_TMAP:
			*_k = a2_nil2obj();
			__v = a2_map_next(a2_gcobj2map(_c->value.obj), _k);
			if(__v==NULL)
				jump(ir_gbx(curr_ir));
			else{
				*_v = *__v;
				curr_pc++;
			}
			break;
		case A2_TARRAY:
			*_k = a2_number2obj(0.0);
			__v = a2_array_next(a2_gcobj2array(_c->value.obj), _k);
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
	struct a2_obj* _k = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _v = a2_closure_arg(curr_cls, ir_ga(curr_ir)+1);
	struct a2_obj* _c = a2_closure_arg(curr_cls, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(_c->type!=A2_TMAP && _c->type!=A2_TARRAY)
		vm_error("the varable is not container.");

	// dump next
	switch(_c->type){
		case A2_TMAP:
			__v = a2_map_next(a2_gcobj2map(_c->value.obj), _k);
			if(__v==NULL)
				curr_pc++;
			else{
				*_v = *__v;
				jump(ir_gbx(curr_ir));
			}
			break;
		case A2_TARRAY:
			__v = a2_array_next(a2_gcobj2array(_c->value.obj), _k);
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
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	*_d = *a2_closure_arg(curr_cls, ir_gbx(curr_ir));
	curr_pc++;
}

// test
static inline void _vm_test(struct a2_vm* vm_p){
	struct  a2_obj* _v = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	if(_v->type==A2_TNIL || (_v->type==A2_TBOOL && !(_v->value.uinteger)))
		jump(ir_gbx(curr_ir));
	else
		curr_pc++;
}

// not 
static inline void _vm_not(struct a2_vm* vm_p){
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	switch(_v->type){
		case A2_TNIL:
			*_d = a2_bool2obj(1);
			break;
		case A2_TBOOL:
			*_d = a2_bool2obj(!(_v->value.uinteger));
			break;
		default:
			vm_error("the varable is not bool type.");
	}

	curr_pc++;
}

// neg
static inline void _vm_neg(struct a2_vm* vm_p){
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	if(_v->type!=A2_TNUMBER)
		vm_error("the varable is not number type at set neg.");
	*_d = a2_number2obj(_v->value.number*(-1));
	curr_pc++;
}

// getupvalue
static inline void _vm_get_upvalue(struct a2_vm* vm_p){
	struct a2_obj* _uv = a2_closure_upvalue(curr_cls, ir_gbx(curr_ir));
	struct a2_obj* _d = a2_closure_arg(curr_cls, ir_ga(curr_ir));

	*_d = *_uv;
	curr_pc++;
}

// set upvalue
static inline void _vm_set_upvalue(struct a2_vm* vm_p){
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));
	struct a2_obj* _ud = a2_closure_upvalue(curr_cls, ir_ga(curr_ir));
	
	*_ud = *_v;
	curr_pc++;
}

// call
static inline void _vm_call(struct a2_vm* vm_p){
	struct a2_obj* _func = a2_closure_arg(curr_cls, ir_ga(curr_ir));
	switch(_func->type){
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
	assert(_func->type==A2_TCFUNCTION);
	int i, j;
	struct a2_obj* _obj = NULL;

	// back bottom
	int _b = a2_getbottom(vm_p->env_p);
	int _top = a2_gettop(vm_p->env_p);

	a2_setbottom(vm_p->env_p, a2_gettop(vm_p->env_p));

	// closure arg to cstack
	for(i=ir_ga(curr_ir)+1; i<=ir_ga(curr_ir)+ir_gb(curr_ir); i++){
		_obj = a2_closure_arg(curr_cls, i);
		a2_pushstack(vm_p->env_p, _obj);
	}

	// call c function
	callinfo_new(vm_p, NULL, 0, 0);
	int ret = _func->value.cfunction(a2_env2state(vm_p->env_p));
	callinfo_free(vm_p);

	// set return value
	for(i=0, j=ir_ga(curr_ir); 
		i<ret && j<ir_ga(curr_ir)+ir_gc(curr_ir);
		j++,i++){
		_obj = a2_closure_arg(curr_cls, j);
		*_obj = *a2_getcstack(vm_p->env_p, i);
	}

	for(; j<ir_ga(curr_ir)+ir_gc(curr_ir); j++){
		_obj = a2_closure_arg(curr_cls, j);
		_obj->type = A2_TNIL;
	}

	a2_setbottom(vm_p->env_p, _b);
	a2_settop(vm_p->env_p, _top);
	curr_pc++;
}

// call a2 function
static inline void __vm_call_function(struct a2_vm* vm_p, struct a2_obj* _func){
	assert(_func->type==A2_TCLOSURE);
	struct a2_obj* _obj = NULL;
	int i, j, params = a2_closure_params(a2_gcobj2closure(_func->value.obj));

	//TODO not allow ...
	assert(params>=0);

	// set params
	for(i=ir_ga(curr_ir)+1, j=0; 
		i<=ir_ga(curr_ir)+ir_gb(curr_ir) && j<params; 
		j++, i++){
		_obj = a2_closure_arg(a2_gcobj2closure(_func->value.obj), j);
		*_obj = *a2_closure_arg(curr_cls, i);
	}
	
	// set clear params
	for( ;j<params; j++){
		_obj = a2_closure_arg(a2_gcobj2closure(_func->value.obj), j);
		_obj->type = A2_TNIL;
	}
	
	// new call info
	int b = ir_ga(curr_ir), n=ir_gc(curr_ir);

	curr_pc++; 
	callinfo_new(vm_p, a2_gcobj2closure(_func->value.obj), b, n);
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
		_obj = a2_closure_arg(curr_cls, i);
		a2_pushstack(vm_p->env_p, _obj);
	}
	ret = ir_gbx(curr_ir);
	callinfo_free(vm_p);
	return ret;
}

// return to a2 function
static inline int __vm_return_function(struct a2_vm* vm_p){
	struct a2_obj* _obj = NULL;
	struct vm_callinfo* call_ci = curr_ci->next;
	struct a2_closure* call_cls = call_ci->cls;
	int i, j, ret;

	// set return
	for(i=ir_ga(curr_ir), j=curr_ci->retbegin; 
		i<ir_ga(curr_ir)+ir_gbx(curr_ir) && j<curr_ci->retbegin+curr_ci->retnumber;
		 j++, i++){
		_obj = a2_closure_arg(call_cls, j);
		*_obj = *a2_closure_arg(curr_cls, i);
	}

	// set clear
	for( ; j<curr_ci->retbegin+curr_ci->retnumber; j++){
		_obj = a2_closure_arg(call_ci->cls, j);
		_obj->type = A2_TNIL;
	}

	ret = curr_ci->retnumber;
	callinfo_free(vm_p);
	return ret;
}

