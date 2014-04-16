#include "a2_conf.h"
#include "a2_env.h"
#include "a2_lex.h"
#include "a2_error.h"
#include "a2_parse.h"
#include "a2_map.h"
#include "a2_array.h"
#include "a2_ir.h"
#include "a2_gc.h"
#include "a2_xclosure.h"
#include "a2_string.h"
#include <stdio.h>
#include <string.h>

#define curr_line   ((ir_p)->cur_line)
#define node_line(n)  (assert(n), ir_p->cur_line=node_p(n)->token->line, ir_p->cur_line)
#define node_p(i) 	  a2_nodep(ir_p->env_p, (i))
#define node_t(i)	  (node_p(i)->type)
#define node_ct(n,i)  (node_p(node_p(n)->childs[i])->type)
#define node_cp(n,i)  (node_p(node_p(n)->childs[i]))

#define ir_error(i, s)  do{a2_error(ir_p->env_p, e_ir_error, "[ir error@line: %zd]: %s\n",node_p(i)->token->line,s);}while(0)
#define curr_sym (ir_p->cls_sym_chain->sym.sym_chain[ir_p->cls_sym_chain->sym.cap-1])
#define curr_csym (ir_p->cls_sym_chain->sym.sym_chain[0])
//#define curr_gsym  (ir_p->cls_sym_chain->sym.sym_chain[1])

#define curr_clssym (ir_p->cls_sym_chain)
#define curr_xcls  	(curr_clssym->xcls)

#define add_arg  _add_arg(ir_p)
#define curr_arg (curr_clssym->arg_cap)
#define set_arg(i) _set_arg(ir_p,i)

#define curr_fs		(&(curr_clssym->fs))
#define add_fors(addr)	_for_stack_add(curr_fs, (addr))

#define curr_fh  (&(curr_clssym->fh))
#define add_forh(addr) _for_stack_add(curr_fh, (addr))

#define top_arg (assert(curr_clssym->arg_cap), curr_clssym->arg_cap-1)
// #define del_arg  _del_arg(ir_p)

#define sym_v(vt,i)	(((vt)<<(30))|(i))
#define v2vt(v)  ((v)>>(30)) 
#define v2i(v)	 (((v)<<2)>>2)
#define DEF_SYM_SIZE 4

#define DEF_XCLS_SIZE	32

// varable type
enum var_type{
	var_local,
	var_global,
	var_upvalue,
	var_hm
};

struct symbol{
	struct a2_map** sym_chain;
	size_t size;
	size_t cap;
};

struct for_stack{
	size_t* f_p;
	size_t len;
	size_t size;
};

struct cls_sym{
	struct symbol  sym;
	struct a2_xclosure* xcls;
	int arg_cap;
	int max_arg;
	struct for_stack fs;
	struct for_stack fh;
	struct cls_sym* next;
};

struct a2_ir{
	struct a2_env* env_p;
	struct cls_sym* cls_sym_chain;

	size_t cur_line;
	byte op_modle[ir_count];
	struct {
		struct a2_xclosure** xcls_chain;
		size_t len;
		size_t size;
	} xcls_stack;
};

#define op_modle(op) (assert(op>=0 && op<ir_count), ir_p->op_modle[op])

static void _init_op_modle(struct a2_ir* ir_p);

static inline int _add_arg(struct a2_ir* ir_p);
// static inline int _del_arg(struct a2_ir* ir_p);
static inline int _set_arg(struct a2_ir* ir_p, int i);

static inline void _for_stack_init(struct for_stack* fs);
static inline void _for_stack_destroy(struct for_stack* fs);
static inline void _for_stack_add(struct for_stack* fs, size_t addr);

static inline void new_symbol(struct a2_ir* ir_p);
static inline void free_symbol(struct a2_ir* ir_p);
static inline void _a2_ir_exec(struct a2_ir* ir_p, struct cls_sym* cls_sp, size_t root);

static struct cls_sym* cls_sym_new(struct a2_ir* ir_p);
static void cls_sym_free(struct a2_ir* ir_p, struct cls_sym* p);

static inline void  new_clssym(struct a2_ir* ir_p);
static inline void free_clssym(struct a2_ir* ir_p);

static inline struct a2_obj node2obj(struct a2_ir* ir_p, size_t node);

static inline void _a2_ir_segment(struct a2_ir* ir_p, size_t root);


typedef int (*rv_func)(struct a2_ir* ir_p, int vt, int idx, size_t right_root);
static inline int a2_ir_wvar(struct a2_ir* ir_p, size_t root, rv_func _rv_func, size_t right_root);
static int _rv_mass(struct a2_ir* ir_p, int vt, int idx, size_t right_root);
static int a2_ir_ass(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_local(struct a2_ir* ir_p, size_t root);
static int _a2_ir_exp(struct a2_ir* ir_p, size_t root, int des);
static inline int a2_ir_exp(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_var(struct a2_ir* ir_p, size_t root, int des);
static inline void a2_ir_if(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_for(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_foreach(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_break(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_continue(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_function(struct a2_ir* ir_p, size_t root, int des);
static inline void a2_ir_return(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_funccall(struct a2_ir* ir_p, size_t root, int ret_count);
static inline int _a2_ir_mass(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_array(struct a2_ir* ir_p, size_t root, int des);
static inline int a2_ir_map(struct a2_ir* ir_p, size_t root, int des);
static inline int a2_ir_neg(struct a2_ir* ir_p, size_t root, int des);
static inline int a2_ir_not(struct a2_ir* ir_p, size_t root, int des);

typedef void (*_cma_func)(struct a2_ir* ir_p, int ctn, int key, int des);
typedef int (*_ir_chm_func)(struct a2_ir* ir_p, size_t root, int ldes, int des, _cma_func cma_func);
static int a2_ir_chi(struct a2_ir* ir_p, size_t root, int ldes, int des, _cma_func cma_func);
static inline int a2_ir_cma(struct a2_ir* ir_p, size_t root, int ldes, int des, _cma_func cma_func);
static void _cma_get(struct a2_ir* ir_p, int ctn, int key, int des);
static void _cma_set(struct a2_ir* ir_p, int ctn, int key, int value);

// return is count
static inline int _a2_ir_comm(struct a2_ir* ir_p, size_t rb);

struct a2_ir* a2_ir_open(struct a2_env* env){
	assert(env);
	assert(ir_count<=(1<<OP_SIZE));
	struct a2_ir* ret = NULL;
	ret = (struct a2_ir*)malloc(sizeof(*ret));
	ret->env_p = env;
	ret->cur_line = 0;
	ret->cls_sym_chain = NULL;
	_init_op_modle(ret);

	ret->xcls_stack.xcls_chain = (struct a2_xclosure**)malloc(
		sizeof(struct a2_xclosure*)*DEF_XCLS_SIZE);
	ret->xcls_stack.size = DEF_XCLS_SIZE;
	ret->xcls_stack.len = 0;
	return ret;
}

void a2_ir_close(struct a2_ir* ir_p){
	if(!ir_p) return;
	while(ir_p->cls_sym_chain){
		struct cls_sym* np = ir_p->cls_sym_chain->next;
		cls_sym_free(ir_p, ir_p->cls_sym_chain);
		ir_p->cls_sym_chain = np;
	}

	free(ir_p->xcls_stack.xcls_chain);
	free(ir_p);
}

static inline void _xcls_push(struct a2_ir* ir_p, struct a2_xclosure* xcls){
	if(ir_p->xcls_stack.len>=ir_p->xcls_stack.size){
		ir_p->xcls_stack.size *= 2;
		ir_p->xcls_stack.xcls_chain = (struct a2_xclosure**)realloc(
			ir_p->xcls_stack.xcls_chain,
			 sizeof(struct a2_xclosure*)*ir_p->xcls_stack.size);
	}

	ir_p->xcls_stack.xcls_chain[ir_p->xcls_stack.len++] = xcls;
}

static inline void _xcls_clear(struct a2_ir* ir_p){
	ir_p->xcls_stack.len = 0;
}

inline void a2_ir_newxcls(struct a2_ir* ir_p){
	new_clssym(ir_p);
}

void a2_ir_clear(struct a2_ir* ir_p){
	ir_p->cur_line = 0;
	while(ir_p->cls_sym_chain){
		struct cls_sym* np = ir_p->cls_sym_chain->next;
		cls_sym_free(ir_p, ir_p->cls_sym_chain);
		ir_p->cls_sym_chain = np;
	}

	ir_p->cls_sym_chain = NULL;
	// clear xclosure
	int i;
	for(i=0; i<ir_p->xcls_stack.len; i++){
		a2_xclosure_free(ir_p->xcls_stack.xcls_chain[i]);
	}
	ir_p->xcls_stack.len =0;
}

static void _init_op_modle(struct a2_ir* ir_p){
	memset(ir_p->op_modle, ABC_MODE, sizeof(ir_p->op_modle));

	ir_p->op_modle[GETGLOBAL] = ABX_MODE;
	ir_p->op_modle[SETUPVALUE] = ABX_MODE;
	ir_p->op_modle[GETUPVALUE] = ABX_MODE;
	ir_p->op_modle[NEWLIST] = ABX_MODE;
	ir_p->op_modle[NEWMAP] = ABX_MODE;
	ir_p->op_modle[CLOSURE] = ABX_MODE;
	ir_p->op_modle[RETURN] = ABX_MODE;
	ir_p->op_modle[FORLOOP] = ABX_MODE;
	ir_p->op_modle[FORPREP] = ABX_MODE;
	ir_p->op_modle[JUMP] = ABX_MODE;
	ir_p->op_modle[MOVE] = ABX_MODE;
	ir_p->op_modle[TEST] = ABX_MODE;
	ir_p->op_modle[LOAD] = ABX_MODE;
	ir_p->op_modle[LOADNIL] = ABX_MODE;
	ir_p->op_modle[NEG] = ABX_MODE;
	ir_p->op_modle[NOT] = ABX_MODE;
	ir_p->op_modle[FOREACHPREP] = ABX_MODE;
	ir_p->op_modle[FOREACHLOOP] = ABX_MODE;
}

static struct cls_sym* cls_sym_new(struct a2_ir* ir_p){
	struct cls_sym* ret = (struct cls_sym*)malloc(sizeof(*ret));
	ret->next = NULL;
	ret->xcls = a2_xclosure_new();
	_xcls_push(ir_p, ret->xcls);

	ret->sym.cap=0;
	ret->sym.size=DEF_SYM_SIZE;
	ret->sym.cap=0;
	ret->arg_cap = 0;
	ret->max_arg = 0;
	ret->sym.sym_chain = (struct a2_map**)malloc(sizeof(struct a2_map*)*DEF_SYM_SIZE);
	return ret;
}

static void cls_sym_free(struct a2_ir* ir_p, struct cls_sym* p){
	int i;
	for(i=0; i<p->sym.cap; i++){
		a2_map_free(p->sym.sym_chain[i]);
	}

	_for_stack_destroy(&(p->fs));
	_for_stack_destroy(&(p->fh));
	free(p->sym.sym_chain);
	// pass cls
	free(p);
}

static inline int _add_arg(struct a2_ir* ir_p){
	if(curr_clssym->arg_cap>=(ARG_MAX-1))
		a2_error(ir_p->env_p, e_ir_error, "the varable more than %d at closure.\n", ARG_MAX);
	if(curr_clssym->arg_cap+1>curr_clssym->max_arg)
		curr_clssym->max_arg = curr_clssym->arg_cap+1;
	return curr_clssym->arg_cap++;
}

// static inline int _del_arg(struct a2_ir* ir_p){
// 	assert(curr_clssym->arg_cap>0);
// 	return curr_clssym->arg_cap--;
// }


static inline int _set_arg(struct a2_ir* ir_p, int i){
	assert(i>=0&&i<ARG_MAX);
	if(i>curr_clssym->max_arg)
		curr_clssym->max_arg = i;
	return curr_clssym->arg_cap = i;
}


static inline void _for_stack_init(struct for_stack* fs){
	fs->size = 8;
	fs->len=0;
	fs->f_p = (size_t*)malloc(sizeof(size_t)*fs->size);
}

static inline void _for_stack_destroy(struct for_stack* fs){
	free(fs->f_p);
}

static inline void _for_stack_add(struct for_stack* fs, size_t addr){
	if(fs->len>=fs->size){
		fs->size *=2;
		fs->f_p = (size_t*)realloc(fs->f_p, fs->size*sizeof(size_t));
	}
	fs->f_p[fs->len++] = addr;
}

inline void a2_ir_exec(struct a2_ir* ir_p, size_t root){
	_a2_ir_exec(ir_p, ir_p->cls_sym_chain, root);
}

inline struct a2_xclosure* a2_ir_exend(struct a2_ir* ir_p){
	assert(ir_p->cls_sym_chain->next==NULL);
	xclosure_add_ir(curr_xcls, ir_abx(RETURN, 0, 0), curr_line);
	a2_xclosure_setregs(curr_xcls, curr_clssym->max_arg);
	_xcls_clear(ir_p);
	return curr_xcls;
}

static inline void _a2_ir_exec(struct a2_ir* ir_p, struct cls_sym* cls_sp, size_t root){
	assert(root);
	int _b = curr_arg;
	// generate intermediate representation
	switch(node_t(root)){
		case local_node:	// local 
			a2_ir_local(ir_p, root);
			break;
		case  ass_node:		// =
			a2_ir_ass(ir_p, root);
			set_arg(_b);
			break;
		case if_node:	// if
			a2_ir_if(ir_p, root);
			break;
		case for_node:	// for
			a2_ir_for(ir_p, root);
			break;
		case foreach_node:	// foreach
			a2_ir_foreach(ir_p, root);
			break;
		case break_node: // break
			a2_ir_break(ir_p, root);
			break;
		case continue_node: // continue
			a2_ir_continue(ir_p, root);
			break;
		case func_node: // function
			a2_ir_function(ir_p, root, add_arg);
			set_arg(_b);
			break;
		case return_node: // return
			a2_ir_return(ir_p, root); 
			break;
		case cfunc_node: // function call
			a2_ir_funccall(ir_p, root, 1);
			set_arg(_b);
			break;
		default:
			ir_error(root, "the expression is  nonsense.");
	}
}

static inline void new_symbol(struct a2_ir* ir_p){
	struct cls_sym* cls_sp = ir_p->cls_sym_chain;
	if(cls_sp->sym.cap>=cls_sp->sym.size){
		cls_sp->sym.size *=2;
		cls_sp->sym.sym_chain = (struct a2_map**)realloc(cls_sp->sym.sym_chain,
			 cls_sp->sym.size*sizeof(struct a2_map*));
	}
	cls_sp->sym.sym_chain[(cls_sp->sym.cap)++] = a2_map_new();
}

static inline void free_symbol(struct a2_ir* ir_p){
	assert(ir_p->cls_sym_chain->sym.cap);
	(ir_p->cls_sym_chain->sym.cap)--;
	struct a2_map* p = ir_p->cls_sym_chain->sym.sym_chain[ir_p->cls_sym_chain->sym.cap];
	a2_map_free(p);
}


static inline void new_clssym(struct a2_ir* ir_p){
	struct cls_sym* np = cls_sym_new(ir_p);

	np->next = ir_p->cls_sym_chain;
	ir_p->cls_sym_chain = np;
	_for_stack_init(&(np->fs));
	_for_stack_init(&(np->fh));
	new_symbol(ir_p); // const symbol
	new_symbol(ir_p); // local symbol
}

static inline void free_clssym(struct a2_ir* ir_p){
	assert(ir_p->cls_sym_chain);
	struct cls_sym* np = ir_p->cls_sym_chain->next;
	cls_sym_free(ir_p, ir_p->cls_sym_chain);
	ir_p->cls_sym_chain = np;
}

// add constant varable symbol
static inline int  add_csymbol(struct a2_ir* ir_p, struct a2_obj* k){
	assert(obj_t(k)==A2_TNUMBER || obj_t(k)==A2_TSTRING 
		|| obj_t(k)==A2_TNIL || obj_t(k)==A2_TBOOL || obj_t(k)==_A2_TADDR);

	struct a2_obj* vp = a2_map_query(curr_csym, k);
	if(!vp){  // not find
		int idx = xclosure_push_cstack(ir_p->env_p, curr_xcls, k); 
		struct a2_obj v = a2_uinteger2obj(idx);
		struct a2_kv kv = {
			k, &v
		};
		a2_map_add(curr_csym, &kv);
		return -1-idx;
	}else
		return -1 - (int)(obj_vX(vp, uinteger));
}

// add bool varable to constent symbol
static inline int add_bool(struct a2_ir* ir_p, int t){
	struct a2_obj k = a2_bool2obj(t);
	return add_csymbol(ir_p, &k);
}

// add nil varable yo constent symbol
static inline int add_nil(struct a2_ir* ir_p){
	struct a2_obj k = a2_nil2obj();
	return add_csymbol(ir_p, &k);
}

// add local varable symbol
static inline int add_lsymbol(struct a2_ir* ir_p, struct a2_obj* k, size_t root){
	assert(obj_t(k)==A2_TSTRING);
	struct a2_obj v = a2_uinteger2obj(sym_v(var_local, curr_clssym->arg_cap));
	struct a2_kv kv = {
		k, &v
	};

	struct a2_obj* vp = NULL;
	if(NULL!=(vp=a2_map_query(curr_sym, k))){
		assert(obj_t(vp)==_A2_TUINTEGER);
		switch(v2vt(obj_vX(vp, uinteger))){
			case var_local:
				ir_error(root, "the varable is volatile.");
				break;
			case var_upvalue:  // if find upvalue, will overload upvalue
				*vp = v;
				break;
			default:
				assert(0);
		}
	}else{
		a2_map_add(curr_sym, &kv);
	}
	
	return _add_arg(ir_p);
}

// get varable symbol return is closure idx+1, vt_p is return enum var_type
static inline int get_symbol(struct a2_ir* ir_p, struct a2_obj* k, int* vt_p){
	assert(obj_t(k)==A2_TSTRING);
	// if local varable , find at curr_clssym
	int i;
	struct a2_obj* vp = NULL;
	// loop sym at cur_cls
	for(i=curr_clssym->sym.cap-1; i>=1; i--){
		vp = a2_map_query(curr_clssym->sym.sym_chain[i], k);
		if(vp){
			assert(obj_t(vp)==_A2_TUINTEGER);
			*vt_p = v2vt(obj_vX(vp, uinteger));
			return v2i(obj_vX(vp, uinteger))+1;
		}
	}

	// not find varable at cur_cls, so find it at up cls_sym
	struct cls_sym* p = curr_clssym->next;
	while(p){
		struct cls_sym* np = p->next;
		for(i=p->sym.cap-1; i>=1; i--){
			vp = a2_map_query(p->sym.sym_chain[i], k);
			if(vp){
				assert(obj_t(vp)==_A2_TUINTEGER);
				int uv_idx;
				struct a2_obj v;
				struct a2_kv kv;
				struct cls_sym* _p = curr_clssym;
				int idx = -1;
				int _uvx;
				switch(v2vt(obj_vX(vp, uinteger))){  // set upvalue
					case var_local:
						_uvx = uvx_reg;
						break;
					case var_upvalue:
						_uvx = uvx_upvalue;
						break;
					default:
						assert(0);
						return 0;
				}
				
				while(_p->next!=p){
					uv_idx = xclosure_push_upvaluex(_p->xcls, uvx_upvalue, xcls_cur_uvx_count(_p->next->xcls));
					if(idx<=-1)
						idx = uv_idx;
					v = a2_uinteger2obj(sym_v(var_upvalue, uv_idx));
					kv.key = k;
					kv.vp = &v;
					a2_map_add(_p->sym.sym_chain[1], &kv);
					_p = _p->next;
				}

				assert(_p->next == p);
				uv_idx = xclosure_push_upvaluex(_p->xcls, _uvx, v2i(obj_vX(vp, uinteger)));
				if(idx<=-1)
					idx = uv_idx;
				v = a2_uinteger2obj(sym_v(var_upvalue, uv_idx));
				kv.key = k;
				kv.vp = &v;
				a2_map_add(_p->sym.sym_chain[1], &kv);
				assert(idx>=0);

				*vt_p = var_upvalue;
				assert(idx>=0);
				return idx+1;
			}
		}
		 p = np;
	}

	//  not find varable at up cls_sym, so it is global varable
	*vt_p = var_global;
	return add_csymbol(ir_p, k);
}

static inline void _a2_ir_segment(struct a2_ir* ir_p, size_t root){
	for(;root;){
		a2_ir_exec(ir_p, root);
		root = node_p(root)->next;
	}
}

static inline int a2_ir_function(struct a2_ir* ir_p, size_t root, int des){
	assert(node_t(root)==func_node);

	size_t arg = node_p(root)->childs[0];
	size_t seg = node_p(root)->childs[1];
	assert(seg);

	int _b = curr_arg;
	// generate args
	new_clssym(ir_p);
	int params=0;
	for(;arg;){
		switch(node_t(arg)){
			case var_node:{
				struct a2_obj k = node2obj(ir_p, arg);
				add_lsymbol(ir_p, &k, arg);
			}
			break;
			case args_node:{
				// multiple parameter
				struct a2_obj _args = a2_env_addstr(ir_p->env_p, "_args");
				assert(obj_t(&_args)==A2_TSTRING);
				add_lsymbol(ir_p, &_args, arg);
				assert(params>=0);
				params = -1 -params;
				goto ARG_FUNC;
			}
			default:
				assert(0);
		}
		params++;
ARG_FUNC:
		arg = node_p(arg)->next;
	}
	a2_xclosure_setparams(curr_xcls, params);
	size_t func_line = (node_p(root)->token)?(node_line(root)):(node_cp(root, 1)->token->line);
	// generater segment
	_a2_ir_segment(ir_p, seg);
	struct a2_xclosure* _xcls = curr_xcls;
	xclosure_add_ir(curr_xcls, ir_abx(RETURN,0,0), curr_line);
	a2_xclosure_setregs(curr_xcls, curr_clssym->max_arg);
	free_clssym(ir_p);

	// set gc stack
	int cls_gcidx = xclosure_push_xcls(ir_p->env_p, curr_xcls, _xcls);
	assert(cls_gcidx>=0);

	xclosure_add_ir(curr_xcls, ir_abx(CLOSURE, des, cls_gcidx), func_line);
	// global function
	if(node_p(root)->token){
		assert(tt2tk(node_p(root)->token->tt)==tk_ide);
		struct a2_obj func;
		obj_setX(&func, A2_TSTRING, obj, node_p(root)->token->v.obj);
		int k = add_csymbol(ir_p, &func);
		assert(k<0);
		if(is_Blimit(k)){
			xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, k), func_line);
			k = top_arg;
		}

		xclosure_add_ir(curr_xcls, ir_abc(SETGLOBAL, des, k, des), func_line);
	}else{	// Anonymous function
		assert(curr_arg<=_b+1);
	}

	set_arg(_b);
	return des;
}

static inline int a2_ir_funccall(struct a2_ir* ir_p, size_t root, int ret_count){
	assert(node_t(root)==cfunc_node);
	assert(node_p(root)->childs[0]);
	assert(ret_count>0 && ret_count<C_MAX);
	int _b = curr_arg;

	// generate function
	int _func = a2_ir_exp(ir_p, node_p(root)->childs[0]);
	assert(_func>=0);
	if(_func<_b){
		xclosure_add_ir(curr_xcls, ir_abx(MOVE, _b, _func), node_line(root));
		_func = _b;
	}else if(_func - _b >0)
		assert(0);
	set_arg(_b+1);

	// generate argument
	size_t args = node_p(root)->childs[1];
	int count = _a2_ir_comm(ir_p, args);
	if(count >=B_MAX)
		ir_error(root, "you set argument is overfllow.");

	xclosure_add_ir(curr_xcls, ir_abc(CALL, _func, count, ret_count), node_line(root));
	curr_clssym->max_arg += ret_count;
	set_arg(_b+1);
	return _b;
}

static inline int  _a2_ir_comm(struct a2_ir* ir_p, size_t rb){
	int count = 0;
	for( ;rb; ){
		int _cb = curr_arg;
		int _rb = a2_ir_exp(ir_p, rb);
		if(_rb<0)
			xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, _rb), node_line(rb));
		else if(_rb<_cb)
			xclosure_add_ir(curr_xcls, ir_abx(MOVE, top_arg, _rb), node_line(rb));
		assert(curr_arg-_cb==1);
		rb = node_p(rb)->next;
		count++;
	}
	return count;
}

static void a2_ir_return(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==return_node);
	assert(node_p(root)->next==0);
	size_t ret = node_p(root)->childs[0];
	int _b = curr_arg;

	// not return varable 
	if(ret==0){
		xclosure_add_ir(curr_xcls, ir_abx(RETURN, 0, 0), node_line(root));
	}else if(node_p(ret)->type == comma_node){  // return a, b, c ...
		size_t rb = node_p(ret)->childs[0];
		assert(node_p(ret)->next==0);
		int count = _a2_ir_comm(ir_p, rb);
		assert(curr_arg-_b>0);
		xclosure_add_ir(curr_xcls, ir_abx(RETURN, _b, count), node_line(root));
	}else{
		// pgenerate return one value
		int _r = a2_ir_exp(ir_p, ret);
		if(_r<0){ // if is constent varable
			assert(curr_arg==_b);
			xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, _r), node_line(root));
			_r = top_arg;
		}
		xclosure_add_ir(curr_xcls, ir_abx(RETURN, _r, 1), node_line(root));
		set_arg(_b);
	}
}

static inline void a2_ir_break(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==break_node);
	if(curr_fh->len==0)
		ir_error(root, "the break is error.");
	// set JUMP 0, 0  1 is break
	size_t addr = xclosure_add_ir(curr_xcls, ir_abx(JUMP, 0, 0), node_line(root));
	_for_stack_add(curr_fs, addr);
}

static inline void a2_ir_continue(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==continue_node);
	if(curr_fh->len==0)
		ir_error(root, "th continue is error.");
	// set JUMP 1, 0  1 is continue
	size_t addr = xclosure_add_ir(curr_xcls, ir_abx(JUMP, 1, 0), node_line(root));
	_for_stack_add(curr_fs, addr);
}


static inline void a2_ir_foreach(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==foreach_node);
	assert(node_ct(root, 0));
	assert(node_ct(root, 1));
	assert(node_ct(root, 2));
	assert(node_ct(root, 3));

	size_t _for_b = curr_fs->len;
	int _b = curr_arg;
	new_symbol(ir_p);
	size_t k_node = node_p(root)->childs[0];
	size_t v_node = node_p(root)->childs[1];
	size_t c_node = node_p(root)->childs[2];
	size_t s_node = node_p(root)->childs[3];

	// prepare key , value , container 
	struct a2_obj l_obj = node2obj(ir_p, k_node);
	a2_assert(add_lsymbol(ir_p, &l_obj, root), ==, _b);

	l_obj = node2obj(ir_p, v_node);
	a2_assert(add_lsymbol(ir_p, &l_obj, root), ==, _b+1);

	int _c = _a2_ir_exp(ir_p, c_node, add_arg);
	assert(_c>=0);
	if( _c<(_b+2) )
		xclosure_add_ir(curr_xcls, ir_abx(MOVE, top_arg, _c), node_line(root));

	// generate foreach ir
	size_t fb = xclosure_add_ir(curr_xcls, ir_abx(FOREACHPREP, _b, 0), node_line(root));
	l_obj = a2_addr2obj(xclosure_curr_iraddr(curr_xcls));
	int b_addr = add_csymbol(ir_p, &l_obj);
	assert(b_addr<0);
	add_forh(-1-b_addr);

	// generate foreach segment
	_a2_ir_segment(ir_p, s_node);
	xclosure_add_ir(curr_xcls, ir_abx(FOREACHLOOP, _b, b_addr), curr_line);

	free_symbol(ir_p);
	set_arg(_b);
	size_t ir_end = xclosure_curr_iraddr(curr_xcls);
	struct a2_obj ao = a2_addr2obj(ir_end);
	int addr = add_csymbol(ir_p, &ao);
	ao = a2_addr2obj(ir_end-1);
	int _addr = add_csymbol(ir_p, &ao);

	//write back ir
	ir* p = xclosure_seek_ir(curr_xcls, fb);
	*p = ir_abx(FOREACHPREP, _b, addr);
	size_t i;
	for(i= _for_b; i<curr_fs->len; i++){
		p = xclosure_seek_ir(curr_xcls, curr_fs->f_p[i]);
		assert(ir_gop(*p)==JUMP);
		if(ir_ga(*p)==0)  // break
			*p = ir_abx(JUMP, 0, addr);
		else if (ir_ga(*p)==1)  // continue
			*p = ir_abx(JUMP, 1, _addr);
		else
			assert(0);
	}
	curr_fs->len = _for_b;	
}

static inline void a2_ir_for(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==for_node);
	assert(node_ct(root, 0)==ass_node);
	assert(node_ct(root, 2)==num_node);
	
	size_t _for_b = curr_fs->len;
	int _b = curr_arg;
	new_symbol(ir_p);
	// add local index
	struct a2_obj k = node2obj(ir_p, node_cp(root, 0)->childs[0]);
	add_lsymbol(ir_p, &k, root);

	// generate initial varable
	a2_assert(a2_ir_ass(ir_p, node_p(root)->childs[0]), ==, _b);
	
	// generate for count 
	int c_idx = _a2_ir_exp(ir_p, node_p(root)->childs[1], add_arg);
	if(c_idx<0){
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, top_arg, c_idx), node_line(root));
	} else if(c_idx<_b+1){
		xclosure_add_ir(curr_xcls, ir_abx(MOVE, top_arg, c_idx), node_line(root));
	}
	assert(top_arg==_b+1);

	// generate for step
	int s_idx = a2_ir_exp(ir_p, node_p(root)->childs[2]);
	assert(s_idx<0);
	xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, s_idx), node_line(root));
	s_idx = top_arg;
	assert(s_idx==_b+2);

	// for
	size_t ti = xclosure_add_ir(curr_xcls, ir_abx(FORPREP, _b, 0), node_line(root));

	//record the for begin addr
	k = a2_addr2obj(xclosure_curr_iraddr(curr_xcls));
	int b_addr = add_csymbol(ir_p, &k);
	assert(b_addr<0);
	add_forh(-1-b_addr);

	// generate segment ir
	_a2_ir_segment(ir_p, node_p(root)->childs[3]);
	xclosure_add_ir(curr_xcls, ir_abx(FORLOOP, _b, b_addr), curr_line);
	free_symbol(ir_p);
	set_arg(_b);
	size_t ir_end = xclosure_curr_iraddr(curr_xcls);
	struct a2_obj ao = a2_addr2obj(ir_end);
	int addr = add_csymbol(ir_p, &ao);
	ao = a2_addr2obj(ir_end-1);
	int _addr = add_csymbol(ir_p, &ao);
	
	//write back ir
	ir* p = xclosure_seek_ir(curr_xcls, ti);
	*p = ir_abx(FORPREP, _b, addr);
	size_t i;
	for(i= _for_b; i<curr_fs->len; i++){
		p = xclosure_seek_ir(curr_xcls, curr_fs->f_p[i]);
		assert(ir_gop(*p)==JUMP);
		if(ir_ga(*p)==0)  // break
			*p = ir_abx(JUMP, 0, addr);
		else if (ir_ga(*p)==1)  // continue
			*p = ir_abx(JUMP, 1, _addr);
		else
			assert(0);
	}
	curr_fs->len = _for_b;	
}

static inline void a2_ir_if(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==if_node);
	size_t logic_node = node_p(root)->childs[0];
	size_t _if_node = node_p(root)->childs[1];
	size_t _else_node = node_p(root)->childs[2];

	assert(logic_node);
	assert(if_node);

	int _b = curr_arg;
	// generate logic ir
	int logic = a2_ir_exp(ir_p, logic_node);
	if(is_Alimit(logic)){ // is constent varable
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, logic), node_line(root));
		logic = top_arg;
	}
	
	// if logic is false will jump
	size_t addr = xclosure_add_ir(curr_xcls, ir_abx(TEST, logic, 0), node_line(root));
	set_arg(_b);

	new_symbol(ir_p);
	while(_if_node){
		a2_ir_exec(ir_p, _if_node);
		_if_node = node_p(_if_node)->next;
	}
	free_symbol(ir_p);
	set_arg(_b);

	size_t end_addr = (_else_node)?(xclosure_add_ir(curr_xcls, ir_abx(JUMP, 0, 0),curr_line)):
									(0);

	// back record addr
	size_t else_addr = xclosure_curr_iraddr(curr_xcls);
	struct a2_obj addr_obj = a2_addr2obj(else_addr);
	int _else_addr = add_csymbol(ir_p, &addr_obj);
	ir* _test_ir = xclosure_seek_ir(curr_xcls, addr);
	*_test_ir = ir_abx(TEST, logic, _else_addr);

	new_symbol(ir_p);
	while(_else_node){
		a2_ir_exec(ir_p, _else_node);
		_else_node = node_p(_else_node)->next;
	}
	free_symbol(ir_p);
	
	if(end_addr){
		size_t ea = xclosure_curr_iraddr(curr_xcls);
		addr_obj = a2_addr2obj(ea);
		int _ea = add_csymbol(ir_p, &addr_obj);
		ir* _jump_ir = xclosure_seek_ir(curr_xcls, end_addr);
		*_jump_ir = ir_abx(JUMP, 0, _ea);
	}
	set_arg(_b);
}


static inline  void a2_ir_local(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==local_node);
	size_t node, _node;
	node = _node = node_p(root)->childs[0];
	size_t n, b;
	struct a2_obj k;
	int top=curr_arg, count=0, _count=0;
	// set nil varable
	for(;_node;){
		n = node_p(_node)->next;
		switch(node_t(_node)){
			case var_node:
				break;
			case ass_node:
				_count++;
				break;
			default:
				assert(0);
		}
		_node = n;
		count++;
	}
	assert(count);
	if(count>_count){
		xclosure_add_ir(curr_xcls, ir_abx(LOADNIL, top, count), node_line(root));
	}

	// add local varable
	for( ;node; ){
		n = node_p(node)->next;
		switch(node_t(node)){
			case var_node:
				b = node;
				obj_setX(&k, A2_TSTRING, obj, node_p(b)->token->v.obj);
				add_lsymbol(ir_p, &k, b);
				break;
			case ass_node:{
				assert(node_p(node_p(node)->childs[0])->type==var_node);
				b = node_p(node)->childs[0];
				obj_setX(&k, A2_TSTRING, obj, node_p(b)->token->v.obj);
				add_lsymbol(ir_p, &k, b);
				a2_ir_ass(ir_p, node);
			}	
				break;
			default:
				assert(0);
		}
		node = n;
	}
}



static inline struct a2_obj node2obj(struct a2_ir* ir_p, size_t node){
	struct a2_obj ret=a2_nil2obj();
	switch(node_t(node)){
		case var_node:
		case str_node:
			obj_setX(&ret, A2_TSTRING, obj, node_p(node)->token->v.obj);
			break;
		case num_node:
			obj_setNum(&ret, node_p(node)->token->v.number);
			break;
		default:
			assert(0); 
	}
	return ret;
}

static inline int a2_ir_wvar(struct a2_ir* ir_p, size_t root, rv_func _rv_func, size_t right_root){
	int _b = curr_arg;
	int r_idx;
	_ir_chm_func chm_func = NULL;

	switch(node_t(root)){
		case var_node:{
			struct a2_obj k = node2obj(ir_p, root);
			int vt = var_global;
			int idx = get_symbol(ir_p, &k, &vt);
			assert(idx);
			if(vt==var_global){	// not find it
				assert(idx<0);
				r_idx = _rv_func(ir_p, var_global, idx, right_root);
				if(is_Blimit(idx)){
					xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, idx), node_line(root));	// set key to reg
					idx = top_arg;
				}

				if(is_Climit(r_idx)){
					xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, r_idx), node_line(root));
					r_idx = top_arg;
				}

				xclosure_add_ir(curr_xcls, ir_abc(SETGLOBAL, _b, idx, r_idx), node_line(root));
				set_arg(_b+1);
				return _b; 
			}else{
				switch(vt){
					case var_local:{
						assert(idx>0);
						int _eb = curr_arg;
						r_idx = _rv_func(ir_p, var_local, idx, right_root);  // parse right exp
						assert(curr_arg<=_eb+1);
						_eb++;
						if(r_idx<0)
							xclosure_add_ir(curr_xcls, ir_abx(LOAD, idx-1, r_idx), node_line(root));
						else if(r_idx!=idx-1)
							xclosure_add_ir(curr_xcls, ir_abx(MOVE, idx-1, r_idx), node_line(root));
						set_arg(_b);
					}
						return idx-1;
					case var_upvalue:
						r_idx = _rv_func(ir_p, var_upvalue, idx, right_root);
						xclosure_add_ir(curr_xcls, ir_abx(SETUPVALUE, idx-1, r_idx), node_line(root));
						if(curr_arg- _b >1){
							assert(_b==r_idx);
							set_arg(_b+1);
						}
						return r_idx;
					default:
						assert(0);
				}
			}
		}
			break;
		case cma_node:
			chm_func = a2_ir_cma;
			goto HM_WVAR;
		case chi_node:
			chm_func = a2_ir_chi;
HM_WVAR:		
		{
			int r_idx = _rv_func(ir_p, var_hm, -1, right_root);
			int r_cma = chm_func(ir_p, root, add_arg, r_idx, _cma_set);
			assert(r_idx==r_cma);
			if(r_cma<_b){
				set_arg(_b);
				return r_cma;
			}else if(r_cma>_b){
				xclosure_add_ir(curr_xcls, ir_abx(MOVE, _b, r_cma), node_line(root));
			}
			set_arg(_b+1);
			return _b;
		}
			break;
		default:
			assert(0);
	}

	assert(0);
	return 0;
}

int _rv_ass(struct a2_ir* ir_p, int vt, int idx, size_t right_root){
	switch(vt){
		case var_global:
		case var_upvalue:
		case var_hm:
			return a2_ir_exp(ir_p, right_root);
		case var_local:
			return _a2_ir_exp(ir_p, right_root, idx-1);
		default:
			assert(0);
	}

	assert(0);
	return 0;
}

static int a2_ir_ass(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==ass_node);
//	int _b = curr_arg;
	size_t ln = node_p(root)->childs[0];
	size_t rn = node_p(root)->childs[1];
	switch( node_t(ln)){
		case chi_node:
		case cma_node:
		case var_node:
			return a2_ir_wvar(ir_p, ln, _rv_ass, rn);
		case comma_node:
			return _a2_ir_mass(ir_p, root);
		default:
			assert(0);
	}

	assert(0);
	return 0;
}

static inline int _comma_count(struct a2_ir* ir_p, size_t root){
	assert(root);
	assert(node_t(root)==comma_node);
	int count=0;
	root = node_p(root)->childs[0];
	for( ;root; ){
		count++;
		root = node_p(root)->next;
	}

	assert(count>1);
	return count;
}

static inline int _a2_ir_mass(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==ass_node);
	assert(node_ct(root, 0)==comma_node);
	size_t ln = node_p(root)->childs[0];
	size_t rn = node_p(root)->childs[1];

	int _b = curr_arg;
	int lcount = _comma_count(ir_p, ln);
	int rcount=0;
	int _vb=curr_arg;
	switch(node_t(rn)){
		case cfunc_node:
			a2_ir_funccall(ir_p, rn, lcount);
			rcount=lcount;
			set_arg(_vb);
			// assert(curr_arg-_vb==1);
			//_vb = top_arg;
			break;
		case comma_node:
			rn = node_p(rn)->childs[0];
			rcount = _a2_ir_comm(ir_p, rn);
			break;
		default:
			assert(0);
	}

	ln = node_p(ln)->childs[0];
	int _ra=-1;
	int _rb = curr_arg;
	for( ;ln && rcount; rcount--){
		_ra = a2_ir_wvar(ir_p, ln, _rv_mass, _vb);
		set_arg(_rb);
		_vb++;
		ln = node_p(ln)->next;
	}

	assert(_ra>=0 && _ra<=_rb);
	if(_ra>_b){
		xclosure_add_ir(curr_xcls, ir_abx(MOVE, _b, _ra), node_line(root));
		set_arg(_b+1);
	}else{
		set_arg(_b);
		_b = _ra;
	}
	return _b;
}

static int _rv_mass(struct a2_ir* ir_p, int vt, int idx, size_t right_root){
	return (int)right_root;
}

static inline int a2_ir_exp(struct a2_ir* ir_p, size_t root){
	return _a2_ir_exp(ir_p, root, -1);
}

static int _a2_ir_exp(struct a2_ir* ir_p, size_t root, int des){
	int op, _b, l_idx, r_idx;
	_ir_chm_func chm_func = NULL;

	switch(node_p(root)->type){
		// arithmetic operation
		case add_node:
			op = ADD;
			goto OP_IR;
		case sub_node:
			op = SUB;
			goto OP_IR;
		case mul_node:
			op = MUL;
			goto OP_IR;
		case div_node:
			op = DIV;
			goto OP_IR;
		case strcat_node:
			op = CAT;
OP_IR:
			_b = curr_arg;
			l_idx = a2_ir_exp(ir_p, node_p(root)->childs[0]); // left op value
			r_idx = a2_ir_exp(ir_p, node_p(root)->childs[1]); // right op value
			if(is_Blimit(l_idx)){
				xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, l_idx), node_line(root));
				l_idx = top_arg;
			}
			if(is_Climit(r_idx)){
				xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, r_idx), node_line(root));
				r_idx = top_arg;
			}
			if(des<0){
				xclosure_add_ir(curr_xcls, ir_abc(op, _b, l_idx, r_idx), node_line(root));
				set_arg(_b+1);
				return _b;
			}else{
				xclosure_add_ir(curr_xcls, ir_abc(op, des, l_idx, r_idx), node_line(root));
				set_arg(_b);
				return des;
			}
		// neg -a
		case neg_node:
			return a2_ir_neg(ir_p, root, (des<0)?(add_arg):(des));
		// varable 	
		case var_node:
			return a2_ir_var(ir_p, root, (des<0)?(add_arg):(des));
		// constant
		case num_node:{
			struct a2_obj k = a2_number2obj(node_p(root)->token->v.number);
			return add_csymbol(ir_p, &k);
		}
		// string
		case str_node:{
			struct a2_obj k;
			obj_setX(&k, A2_TSTRING, obj, node_p(root)->token->v.obj);
			return add_csymbol(ir_p, &k);
		}
			break;
		// bool 
		case bool_node:
			return add_bool(ir_p, tt2op(node_p(root)->token->tt));
		// nil
		case nil_node:
			return add_nil(ir_p);
		// assignment operation	
		case ass_node:
			return a2_ir_ass(ir_p, root);
		// function 
		case func_node:
			return a2_ir_function(ir_p, root, (des<0)?(add_arg):(des));
		// call function
		case cfunc_node:
			return a2_ir_funccall(ir_p, root, 1);
		// a[k]
		case cma_node:
			chm_func = a2_ir_cma;
			goto CHM_EXP;
		//a. b
		case chi_node:
			chm_func = a2_ir_chi;
CHM_EXP:
		{
			int ldes , _des;
			_des = ldes = (des<0)?(add_arg):(des);
			return chm_func(ir_p, root, ldes, _des,  _cma_get);
		}
		// array
		case array_node:
			return a2_ir_array(ir_p, root, (des<0)?(add_arg):(des));
		// map
		case map_node:
			return a2_ir_map(ir_p, root, (des<0)?(add_arg):(des));
		 // logic operation
		case not_node:
			return a2_ir_not(ir_p, root, (des<0)?(add_arg):(des));
		case or_node:
			op = OR;
			goto OP_IR;
		case and_node:
			op = AND;
			goto OP_IR;
		case gt_node:
			op = BIG;
			goto OP_IR;
		case lt_node:
			op = LITE;
			goto OP_IR;
		case gte_node:
			op = LITEE;
			goto OP_IR;
		case lte_node:
			op = BIGE;
			goto OP_IR;
		case equ_node:
			op = EQU;
			goto OP_IR;
		case ne_node:
			op = NEQU;
			goto OP_IR;
		default:
			assert(0);
	}
	assert(0);
	return 0;
}

// parse not
static inline int a2_ir_not(struct a2_ir* ir_p, size_t root, int des){
	assert(node_t(root)==not_node);
	assert(node_p(root)->childs[0]);
	assert(des>=0);

	int not_v = _a2_ir_exp(ir_p, node_p(root)->childs[0], des);
	xclosure_add_ir(curr_xcls, ir_abx(NOT, des, not_v), node_line(root));
	return des;
}

// parse neg
static inline int a2_ir_neg(struct a2_ir* ir_p, size_t root, int des){
	assert(node_t(root)==neg_node);
	assert(node_p(root)->childs[0]);
	assert(des>=0);

	int neg_v = _a2_ir_exp(ir_p, node_p(root)->childs[0], des);
	xclosure_add_ir(curr_xcls, ir_abx(NEG, des, neg_v), node_line(root));
	return des;
}


// parse map
static inline int a2_ir_map(struct a2_ir* ir_p, size_t root, int des){
	assert(node_t(root)==map_node);
	size_t kv_node = node_p(root)->childs[0];

	int _b = curr_arg, count=0;
	xclosure_add_ir(curr_xcls, ir_abx(NEWMAP, des, 0), node_line(root));
	int _kv_idx = curr_arg;

	for( ;kv_node; ){
		size_t v = node_p(kv_node)->childs[0];
		int _k=0, _v=0;
		int _kvb = curr_arg;

		// generate key
		switch(node_t(kv_node)){
			// constant
			case num_node:
			case var_node:
			case str_node:{
				struct a2_obj k = node2obj(ir_p, kv_node);
				_k = add_csymbol(ir_p, &k);
			}
				break;
			default:
				assert(0);
		} 

		// set key
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, _k), node_line(kv_node));
		// set value
		_v = a2_ir_exp(ir_p, v);
		if(_v<0)
			xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, _v), node_line(v));
		else if (_v<top_arg)
			xclosure_add_ir(curr_xcls, ir_abx(MOVE, top_arg, _v), node_line(v));
		assert(curr_arg-_kvb==2);
		_kvb++;

		kv_node = node_p(kv_node)->next;
		count++;
	}

	assert(curr_arg-_kv_idx==count*2);
	xclosure_add_ir(curr_xcls, ir_abc(SETMAP, des, _kv_idx, count), curr_line);
	set_arg(_b);
	return des;
}

// parse array
static inline int a2_ir_array(struct a2_ir* ir_p, size_t root, int des){
	assert(node_t(root)==array_node);

	int _b = curr_arg;
	xclosure_add_ir(curr_xcls, ir_abx(NEWLIST, des, 0), node_line(root));
	size_t vn = node_p(root)->childs[0];

	// generate value 
	int _vb=curr_arg;
	int count = _a2_ir_comm(ir_p, vn);
	assert(curr_arg-_b==count);
	if(count>0)
		xclosure_add_ir(curr_xcls, ir_abc(SETLIST, des, _vb, count), curr_line);
	set_arg(_b);
	return des;
}

// parse get map value by key
static int a2_ir_chi(struct a2_ir* ir_p, size_t root, int ldes, int des, _cma_func cma_func){
	assert(node_t(root)==chi_node);
	assert(node_p(root)->childs[0]);
	assert(node_p(root)->childs[1]);
	assert(ldes>=0);

	size_t fn = node_p(root)->childs[0];
	size_t cn = node_p(root)->childs[1];
	int _b = curr_arg;
	int f, c;
	switch(node_t(fn)){
		case chi_node:
			f = a2_ir_chi(ir_p, fn, ldes, des, _cma_get);
			break;
		default:
			f = _a2_ir_exp(ir_p, fn, ldes);
	}

	assert(node_t(cn)==var_node);
	struct a2_obj k = node2obj(ir_p, cn);
	c = add_csymbol(ir_p, &k);
	assert(c<0);

	cma_func(ir_p, f, c, des);
	set_arg(_b);
	return des;
}

//parse get map and array  value
static inline int a2_ir_cma(struct a2_ir* ir_p, size_t root, int ldes, int des, _cma_func cma_func){
	assert(node_t(root)==cma_node);
	assert(node_p(root)->childs[0]);
	assert(node_p(root)->childs[1]);

	size_t ctn_node = node_p(root)->childs[0];
	size_t key_node = node_p(root)->childs[1];
	int _b = curr_arg;

	curr_line = node_line(root);
	
	// generate get container obj
	assert(ldes>=0);
	int ctn_idx = _a2_ir_exp(ir_p, ctn_node, ldes);
	assert(ctn_idx>=0 && curr_arg<=_b+1);

	// generate get key 
	int key_idx = a2_ir_exp(ir_p, key_node);
	assert(curr_arg<=_b+2);

	cma_func(ir_p, ctn_idx, key_idx, des);
	set_arg(_b);
	return des;
}

static void _cma_get(struct a2_ir* ir_p, int ctn, int key, int des){
	assert(ctn>=0 && des>=0);
	if(is_Climit(key)){
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, key), curr_line);
		key = top_arg;
	}
	xclosure_add_ir(curr_xcls, ir_abc(GETVALUE, des, ctn, key), curr_line);
}

static void _cma_set(struct a2_ir* ir_p, int ctn, int key, int value){
	assert(ctn>=0);
	if(is_Climit(key)){
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, key), curr_line);
		key = top_arg;
	}
	if(is_Climit(value)){
		xclosure_add_ir(curr_xcls, ir_abx(LOAD, add_arg, value), curr_line);
		value = top_arg;
	}
	xclosure_add_ir(curr_xcls, ir_abc(SETVALUE, ctn, key, value), curr_line);
}

// parse varable
static inline int a2_ir_var(struct a2_ir* ir_p, size_t root, int des){
	struct a2_obj k = node2obj(ir_p, root);
	int vt;
	int idx = get_symbol(ir_p, &k, &vt);
	assert(des>=0);
	if(!idx){
		int k_idx = add_csymbol(ir_p, &k);
		xclosure_add_ir(curr_xcls, ir_abx(GETGLOBAL, des, k_idx), node_line(root));
		return des;
	}else{
		switch(vt){
			case var_global:
				xclosure_add_ir(curr_xcls, ir_abx(GETGLOBAL, des, idx), node_line(root));
				return des;
			case var_local:
				return idx-1;
			case var_upvalue:
				xclosure_add_ir(curr_xcls, ir_abx(GETUPVALUE, des, idx-1), node_line(root));
				return des;
			default:
				assert(0);
		}
	}

	assert(0);
	return 0;
}



// for test 
char* ir2string(struct a2_ir* ir_p, struct a2_xclosure* xcls, ir _ir, char* str, size_t size){
	static char* _irs[] = {
		"NIL",
		"GETGLOBAL",  	// get global variable
		"SETGLOBAL",  	// set global variable 
		"GETUPVALUE",	// get upvalue
		"SETUPVALUE",	// set upvalue
		"NEWLIST",	// new list
		"NEWMAP",	// new map
		"SETLIST",  // set list
		"SETMAP", 	// set map
		"GETVALUE", // get value
		"SETVALUE", // set value
		"CLOSURE",	// closure
		"CALL",		// call
		"RETURN",	// return
		"FORPREP",	// for prepare modle is abx
		"FORLOOP",	// for loop modle is abx
		"FOREACHPREP",// foreach  prep modle is abx 
		"FOREACHLOOP",// foreach loop modle is abx
		"JMP",		// jump
		"MOVE",		// move
		"TEST",		// test
		"LOAD",	 	// load const value to register
		"LOADNIL",	// reset regs is nil
		"CAT",		// ..
		"ADD",	 	// +
		"SUB",	 	// -
		"MUL",		// *
		"DIV", 		// /
		"OR",		// |
		"AND",		// &
		"BIG",		// >
		"LITE",		// <
		"EQU",		// == 
		"NEQU",		// !=
		"BIGE",		// >=
		"LITEE",	// <=
		"NOT",		// !
		"NEG"
	};

	int op = ir_gop(_ir);
	assert(op>=0 && op<(sizeof(_irs)/sizeof(char*)));

	char* ops = _irs[op];
	int a = ir_ga(_ir);
	switch(op_modle(op)){
		case ABC_MODE:{
			int b = ir_gb(_ir);
			int c = ir_gc(_ir);
			char b_buf[32] = {0};
			char* bs = (b<0)?(obj2str(xclosure_at_cstack(xcls, b), b_buf, sizeof(b_buf)-1)):(NULL);
			char c_buf[32] = {0};
			char* cs = (c<0)?(obj2str(xclosure_at_cstack(xcls, c), c_buf, sizeof(c_buf)-1)):(NULL);
			
			int cap = snprintf(str, size, "%s  %d  %d  %d;",ops, a, ir_gb(_ir), ir_gc(_ir));
			if(bs)
				cap += snprintf(str+cap, size-cap, "%s ", bs);
			if(cs)
				snprintf(str+cap, size-cap, "%s", cs);
		}
			break;
		case ABX_MODE:{
			int bx = ir_gbx(_ir);
			char bx_buf[32] = {0};
			char*  bxs = (bx<0)?(obj2str(xclosure_at_cstack(xcls, bx), bx_buf, sizeof(bx_buf)-1)):(NULL);
			int cap = snprintf(str, size, "%s  %d  %d;", ops, a, ir_gbx(_ir));
			if(bxs)
				snprintf(str+cap, size-cap, "%s", bxs);
		}
			break;
		default:
			assert(0);
	}

	// printf("%s", str);
	// exit(0);
	return str;
}

void dump_ir(struct a2_ir* ir_p){
	assert(ir_p);

	// only current
	printf("ir arg=%d\n", curr_clssym->arg_cap);
	dump_xclosure(ir_p, curr_xcls);
}





