#include "a2_conf.h"
#include "a2_env.h"
#include "a2_lex.h"
#include "a2_error.h"
#include "a2_parse.h"
#include "a2_map.h"
#include "a2_ir.h"
#include "a2_gc.h"
#include "a2_closure.h"
#include "a2_string.h"
#include <stdio.h>

#define node_p(i) 	  a2_nodep(ir_p->env_p, (i))
#define node_t(i)	  (node_p(i)->type)
#define node_ct(n,i)  (node_p(node_p(n)->childs[i])->type)
#define node_cp(n,i)  (node_p(node_p(n)->childs[i]))

#define ir_error(i, s)  do{a2_error("[ir error@line: %lu]: %s\n",node_p(i)->token->line,s);}while(0)
#define curr_sym (ir_p->cls_sym_chain->sym.sym_chain[ir_p->cls_sym_chain->sym.cap-1])
#define curr_csym (ir_p->cls_sym_chain->sym.sym_chain[0])
#define curr_clssym (ir_p->cls_sym_chain)
#define curr_cls  a2_gcobj2closure(curr_clssym->cls_obj.value.obj)
#define curr_ocls(co) a2_gcobj2closure((co).value.obj)

#define add_arg  _add_arg(ir_p)
#define curr_arg (curr_clssym->arg_cap)
#define set_arg(i) _set_arg(ir_p,i)

#define curr_fs		(&(curr_clssym->fs))
#define add_fors(addr)	_for_stack_add(curr_fs, (addr))

#define curr_fh  (&(curr_clssym->fh))
#define add_forh(addr) _for_stack_add(curr_fh, (addr))

#define top_arg (assert(curr_clssym->arg_cap), curr_clssym->arg_cap-1)
#define del_arg  _del_arg(ir_p)

#define sym_v(vt,i)	(((vt)<<(30))|(i))
#define v2vt(v)  ((v)>>(30)) 
#define v2i(v)	 (((v)<<2)>>2)
#define DEF_SYM_SIZE 4

// varable type
enum var_type{
	var_local,
	var_global,
	var_upvalue
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
	struct a2_obj cls_obj;
	int arg_cap;
	int max_arg;
	struct for_stack fs;
	struct for_stack fh;
	struct cls_sym* next;
};

struct a2_ir{
	struct a2_env* env_p;
	struct cls_sym* cls_sym_chain;
	struct a2_map* global_sym;
};

static inline int _add_arg(struct a2_ir* ir_p);
static inline int _del_arg(struct a2_ir* ir_p);
static inline int _set_arg(struct a2_ir* ir_p, int i);

static inline void _for_stack_init(struct for_stack* fs);
static inline void _for_stack_destroy(struct for_stack* fs);
static inline void _for_stack_add(struct for_stack* fs, size_t addr);

static inline void new_symbol(struct a2_ir* ir_p);
static inline void free_symbol(struct a2_ir* ir_p);
static inline void _a2_ir_exec(struct a2_ir* ir_p, struct cls_sym* cls_sp, size_t root);

static struct cls_sym* cls_sym_new(struct a2_ir* ir_p);
static void cls_sym_free(struct cls_sym* p);

static inline void  new_clssym(struct a2_ir* ir_p);
static inline void free_clssym(struct a2_ir* ir_p);

static inline struct a2_obj node2obj(struct a2_ir* ir_p, size_t node);

static inline void _a2_ir_segment(struct a2_ir* ir_p, size_t root);

static int a2_ir_ass(struct a2_ir* ir_p, size_t root);
static void a2_ir_local(struct a2_ir* ir_p, size_t root);
static int a2_ir_exp(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_var(struct a2_ir* ir_p, size_t root);
static void a2_ir_if(struct a2_ir* ir_p, size_t root);
static void a2_ir_for(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_break(struct a2_ir* ir_p, size_t root);
static inline void a2_ir_continue(struct a2_ir* ir_p, size_t root);
static int a2_ir_function(struct a2_ir* ir_p, size_t root);

struct a2_ir* a2_ir_open(struct a2_env* env){
	assert(env);
	struct a2_ir* ret = NULL;
	ret = (struct a2_ir*)malloc(sizeof(*ret));
	ret->env_p = env;
	new_clssym(ret);
	ret->global_sym = a2_map_new();
	return ret;
}

void a2_ir_close(struct a2_ir* ir_p){
	if(!ir_p) return;
	while(ir_p->cls_sym_chain){
		struct cls_sym* np = ir_p->cls_sym_chain->next;
		cls_sym_free(ir_p->cls_sym_chain);
		ir_p->cls_sym_chain = np;
	}
	a2_map_free(ir_p->global_sym);
	free(ir_p);
}

static struct cls_sym* cls_sym_new(struct a2_ir* ir_p){
	struct cls_sym* ret = (struct cls_sym*)malloc(sizeof(*ret));
	ret->next = NULL;
	ret->cls_obj.type = A2_TCLOSURE;
	ret->cls_obj.value.obj = a2_closure2gcobj(a2_closure_new());
	a2_gcadd(ir_p->env_p, ret->cls_obj.value.obj);

	ret->sym.cap=0;
	ret->sym.size=DEF_SYM_SIZE;
	ret->sym.cap=0;
	ret->arg_cap = 0;
	ret->max_arg = 0;
	ret->sym.sym_chain = (struct a2_map**)malloc(sizeof(struct a2_map*)*DEF_SYM_SIZE);
	return ret;
}

static void cls_sym_free(struct cls_sym* p){
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
		a2_error("the varable more than %d at closure.\n", ARG_MAX);
	if(curr_clssym->arg_cap+1>curr_clssym->max_arg)
		curr_clssym->max_arg = curr_clssym->arg_cap+1;
	return curr_clssym->arg_cap++;
}

static inline int _del_arg(struct a2_ir* ir_p){
	assert(curr_clssym->arg_cap>0);
	return curr_clssym->arg_cap--;
}

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
		fs->f_p = (size_t*)realloc(fs->f_p, fs->size);
	}
	fs->f_p[fs->len++] = addr;
}

inline void a2_ir_exec(struct a2_ir* ir_p, size_t root){
	_a2_ir_exec(ir_p, ir_p->cls_sym_chain, root);
}


static inline void _a2_ir_exec(struct a2_ir* ir_p, struct cls_sym* cls_sp, size_t root){
	assert(root);

	// generate intermediate representation
	switch(node_t(root)){
		case local_node:	// local 
			a2_ir_local(ir_p, root);
			break;
		case  ass_node:{		// =
			int _b = curr_arg;
			a2_ir_ass(ir_p, root);
			set_arg(_b);
		}
			break;
		case if_node:	// if
			a2_ir_if(ir_p, root);
			break;
		case for_node:	// for
			a2_ir_for(ir_p, root);
			break;
		case break_node: // break
			a2_ir_break(ir_p, root);
			break;
		case continue_node: // continue
			a2_ir_continue(ir_p, root);
			break;
		case func_node: // function
			a2_ir_function(ir_p, root);
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
			 cls_sp->sym.size);
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
	cls_sym_free(ir_p->cls_sym_chain);
	ir_p->cls_sym_chain = np;
}

// add constant varable symbol
static inline int  add_csymbol(struct a2_ir* ir_p, struct a2_obj* k){
	assert(k->type==A2_TNUMBER || k->type==A2_TSTRING 
		|| k->type==A2_TNIL || k->type==A2_TBOOL || k->type==_A2_TADDR);

	struct a2_obj* vp = a2_map_query(curr_csym, k);
	if(!vp){  // not find
		int idx = closure_push_cstack(curr_cls, k); 
		struct a2_obj v = a2_uinteger2obj(idx);
		struct a2_kv kv = {
			k, &v
		};
		assert(a2_map_add(curr_csym, &kv)==a2_true);
		return -1-idx;
	}else
		return -1 - (int)(vp->value.uinteger);
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
	assert(k->type==A2_TSTRING);
	struct a2_obj v = a2_uinteger2obj(sym_v(var_local, curr_clssym->arg_cap));
	struct a2_kv kv = {
		k, &v
	};
	if(a2_map_add(curr_sym, &kv) == a2_fail){
		ir_error(root, "the varable is volatile.");
	}
	return curr_clssym->arg_cap++;
}

// add global varable symbol
static inline int add_gsymbol(struct a2_ir* ir_p, struct a2_obj* k){
	assert(k->type==A2_TSTRING);
	int idx = add_csymbol(ir_p, k);
	struct a2_obj v = a2_uinteger2obj(sym_v(var_global, idx));
	struct a2_kv kv = {
		k, &v
	};
	assert(a2_map_add(ir_p->global_sym, &kv)==a2_true);
	return idx;
}

// get varable symbol return is closure idx+1, vt_p is return enum var_type
static inline int get_symbol(struct a2_ir* ir_p, struct a2_obj* k, int* vt_p){
	assert(k->type==A2_TSTRING);
	// if local varable , find at curr_clssym
	int i;
	struct a2_obj* vp = NULL;
	// loop sym at cur_cls
	for(i=curr_clssym->sym.cap-1; i>=1; i--){
		vp = a2_map_query(curr_clssym->sym.sym_chain[i], k);
		if(vp){
			assert(vp->type==_A2_TUINTEGER);
			*vt_p = v2vt(5);
			return v2i(vp->value.uinteger)+1;
		}
	}

	// not find varable at cur_cls, so find it at up cls_sym
	struct cls_sym* p = curr_clssym->next;
	while(p){
		struct cls_sym* np = p->next;
		for(i=p->sym.cap-1; i>=1; i--){
			vp = a2_map_query(p->sym.sym_chain[i], k);
			if(vp){
				assert(vp->type==_A2_TUINTEGER);
				if(v2vt(vp->value.uinteger)==var_local || v2vt(vp->value.uinteger)==var_upvalue){		// set upvalue
					*vt_p = var_upvalue;
					int idx = closure_push_upvalue(curr_cls, curr_ocls(p->cls_obj), v2i(vp->value.uinteger));
					struct a2_obj v = a2_uinteger2obj(sym_v(var_upvalue, idx));
					struct a2_kv kv = {
						k, &v
					};
					assert(a2_map_add(curr_sym, &kv)==a2_true);
					return idx+1;
				}else{
					assert(0);
				}
			}
		}
		 p = np;
	}

	//  not find varable at up cls_sym , so find it at global sym
	vp = a2_map_query(ir_p->global_sym, k);
	if(vp){
		*vt_p = var_global;
		assert(vp->type==_A2_TUINTEGER);
		return vp->value.uinteger;
	}

	// not find it
	return 0;
}

static inline void _a2_ir_segment(struct a2_ir* ir_p, size_t root){
	for(;root;){
		a2_ir_exec(ir_p, root);
		root = node_p(root)->next;
	}
}

static int a2_ir_function(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==func_node);

	size_t arg = node_p(root)->childs[0];
	size_t seg = node_p(root)->childs[1];
	assert(seg);

	int _b = curr_arg;
	// generate args
	new_clssym(ir_p);
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
				assert(_args.type==A2_TSTRING);
				add_lsymbol(ir_p, &_args, arg);
			}
			break;
			default:
				assert(0);
		}
		arg = node_p(arg)->next;
	}

	// generater segment
	_a2_ir_segment(ir_p, seg);
	struct a2_obj func_obj = curr_clssym->cls_obj;
	closure_add_ir(curr_cls, ir_abx(RETURN,0,0));
	free_clssym(ir_p);

	// set gc stack
	int cls_gcidx = closure_push_gcstack(curr_cls, &func_obj);
	assert(cls_gcidx>=0);

	closure_add_ir(curr_cls, ir_abx(CLOSURE, add_arg, cls_gcidx));
	// global function
	if(node_p(root)->token){
		assert(tt2tk(node_p(root)->token->tt)==tk_ide);
		int v = top_arg;
		struct a2_obj func = {
			A2_TSTRING,
			node_p(root)->token->v
		};
		int k = add_gsymbol(ir_p, &func);
		assert(k<0);
		if(is_Blimit(k)){
			closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, k));
			k = top_arg;
		}

		closure_add_ir(curr_cls, ir_abc(SETGLOBAL, _b, k, v));
	}else{	// Anonymous function
		assert(curr_arg==_b+1);
	}
	set_arg(_b+1);
	return top_arg;
}

static inline void a2_ir_break(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==break_node);
	if(curr_fh->len==0)
		ir_error(root, "the break is error.");
	size_t addr = closure_add_ir(curr_cls, ir_abx(JUMP, 0, 0));
	_for_stack_add(curr_fs, addr);
}

static inline void a2_ir_continue(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==continue_node);
	if(curr_fh->len==0)
		ir_error(root, "th continue is error.");
	size_t _addr = curr_fh->f_p[curr_fh->len-1];
	int addr = -1-_addr;
	closure_add_ir(curr_cls, ir_abx(JUMP, 0, addr));
}


static void a2_ir_for(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==for_node);
	assert(node_ct(root, 0)==var_node);
	assert(node_ct(root, 2)==num_node);
	
	size_t _for_b = curr_fs->len;
	int _b = curr_arg;
	new_symbol(ir_p);
	// add local index
	struct a2_obj k = node2obj(ir_p, node_p(root)->childs[0]);
	int idx = add_lsymbol(ir_p, &k, root);

	//record the for begin addr
	k = a2_addr2obj(closure_curr_iraddr(curr_cls));
	int b_addr = add_csymbol(ir_p, &k);
	assert(b_addr<0);
	add_forh(-1-b_addr);

	// generate logic ir
	int logic = a2_ir_exp(ir_p, node_p(root)->childs[1]);
	// add step number
	k = node2obj(ir_p, node_p(root)->childs[2]);
	int step = add_csymbol(ir_p, &k);

	// set test ir
	if(is_Alimit(logic)){
		closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, logic));
		logic = top_arg;
	}

	size_t ti = closure_add_ir(curr_cls, ir_abx(TEST, logic, 0));

	// generate segment ir
	_a2_ir_segment(ir_p, node_p(root)->childs[3]);
	closure_add_ir(curr_cls, ir_abx(INC, idx, step));
	closure_add_ir(curr_cls, ir_abx(JUMP, 0, b_addr));
	free_symbol(ir_p);
	set_arg(_b);
	size_t ir_end = closure_curr_iraddr(curr_cls);
	struct a2_obj ao = a2_addr2obj(ir_end);
	int addr = add_csymbol(ir_p, &ao);
	
	//write back ir
	ir* p = closure_seek_ir(curr_cls, ti);
	*p = ir_abx(TEST, logic, addr);
	size_t i;
	for(i= _for_b; i<curr_fs->len; i++){
		p = closure_seek_ir(curr_cls, curr_fs->f_p[i]);
		assert(ir_gop(*p)==JUMP);
		*p = ir_abx(JUMP, 0, addr);
	}
	curr_fs->len = _for_b;	
}

static void a2_ir_if(struct a2_ir* ir_p, size_t root){
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
		closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, logic));
		logic = top_arg;
	}
	
	// if logic is false will jump
	size_t addr = closure_add_ir(curr_cls, ir_abx(TEST, logic, 0));
	set_arg(_b);

	new_symbol(ir_p);
	while(_if_node){
		a2_ir_exec(ir_p, _if_node);
		_if_node = node_p(_if_node)->next;
	}
	free_symbol(ir_p);
	set_arg(_b);

	// back record addr
	size_t else_addr = closure_curr_iraddr(curr_cls);
	struct a2_obj addr_obj = a2_addr2obj(else_addr);
	int _else_addr = add_csymbol(ir_p, &addr_obj);
	ir* _test_ir = closure_seek_ir(curr_cls, addr);
	*_test_ir = ir_abx(TEST, logic, _else_addr);

	new_symbol(ir_p);
	while(_else_node){
		a2_ir_exec(ir_p, _else_node);
		_else_node = node_p(_else_node)->next;
	}
	free_symbol(ir_p);
	set_arg(_b);
}


static void a2_ir_local(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==local_node);
	size_t node = node_p(root)->childs[0];
	size_t n, b;
	struct a2_obj k;
	while(node){
		n = node_p(node)->next;
		switch(node_t(node)){
			case var_node:
				b = node;
				k.type = A2_TSTRING;
				k.value = node_p(b)->token->v;
				add_lsymbol(ir_p, &k, b);
				break;
			case ass_node:{
				assert(node_p(node_p(node)->childs[0])->type==var_node);
				b = node_p(node)->childs[0];		
				k.type = A2_TSTRING;
				k.value = node_p(b)->token->v;
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
	struct a2_obj ret;
	switch(node_t(node)){
		case var_node:
		case str_node:
			ret.type = A2_TSTRING;
			ret.value = node_p(node)->token->v;
			break;
		case num_node:
			ret.type = A2_TNUMBER;
			ret.value = node_p(node)->token->v;
			break;
		default:
			assert(0); 
	}
	return ret;
}

static int a2_ir_ass(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==ass_node);
	int _b = curr_arg;
	int r_idx = a2_ir_exp(ir_p, node_p(root)->childs[1]);  // parse right exp

	switch( node_t(node_p(root)->childs[0])){
		case var_node:{
			struct a2_obj k = node2obj(ir_p, node_p(root)->childs[0]);
			int vt;
			int idx = get_symbol(ir_p, &k, &vt);
			if(!idx || vt==var_global){	// not find it
				if(!idx)
					idx = add_gsymbol(ir_p, &k);
				assert(idx<0);
				closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, idx));	// set key to reg
				int k = top_arg;
				int d = top_arg;
				if(is_Climit(r_idx)){
					closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, r_idx));
					r_idx = top_arg;
				}else if(curr_arg - _b>1){
					d = r_idx;
				}

				closure_add_ir(curr_cls, ir_abc(SETGLOBAL, d, k, r_idx));
				set_arg(_b+1);
				return d; 
			}else{
				switch(vt){
					case var_local:
						assert(idx>0);
						closure_add_ir(curr_cls, ir_abx(LOAD, idx-1, r_idx));
						set_arg(_b);
						return idx-1;
					case var_upvalue:
						closure_add_ir(curr_cls, ir_abx(SETUPVALUE, idx-1, r_idx));
						if(curr_arg- _b >1)
							set_arg(_b+1);
						return r_idx;
					default:
						assert(0);
				}
			}
		}
			break;
		default:
			assert(0);
	}

	assert(0);
}

static int a2_ir_exp(struct a2_ir* ir_p, size_t root){
	int op, _b, l_idx, r_idx;
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
OP_IR:
			_b = curr_arg;
			l_idx = a2_ir_exp(ir_p, node_p(root)->childs[0]); // left op value
			r_idx = a2_ir_exp(ir_p, node_p(root)->childs[1]); // right op value
			if(is_Blimit(l_idx)){
				closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, l_idx));
				l_idx = top_arg;
			}
			if(is_Climit(r_idx)){
				closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, r_idx));
				r_idx = top_arg;
			}
			closure_add_ir(curr_cls, ir_abc(op, _b, l_idx, r_idx));
			set_arg(_b+1);
			return _b;
		// varable 	
		case var_node:
			return a2_ir_var(ir_p, root);
		// constant
		case num_node:{
			struct a2_obj k = a2_number2obj(node_p(root)->token->v.number);
			return add_csymbol(ir_p, &k);
		}
		// string
		case str_node:{
			struct a2_obj k = {
				A2_TSTRING, node_p(root)->token->v
			};
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
			return a2_ir_function(ir_p, root);
		 // logic operation
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
}



// parse varable
static inline int a2_ir_var(struct a2_ir* ir_p, size_t root){
	struct a2_obj k = node2obj(ir_p, root);
	int vt;
	int idx = get_symbol(ir_p, &k, &vt);
	if(!idx){
		int k_idx = add_csymbol(ir_p, &k);
		closure_add_ir(curr_cls, ir_abx(GETGLOBAL, add_arg, k_idx));
		return top_arg;
	}else{
		switch(vt){
			case var_global:
				closure_add_ir(curr_cls, ir_abx(GETGLOBAL, add_arg, idx));
				return top_arg;
			case var_local:
				return idx-1;
			case var_upvalue:
				closure_add_ir(curr_cls, ir_abx(GETUPVALUE, add_arg, idx-1));
				return top_arg;
			default:
				assert(0);
		}
	}
}



// for test 
char* ir2string(struct a2_closure* cls, ir _ir, char* str, size_t size){
	static char* _irs[] = {
		"NIL",
		"GETGLOBAL",  // get global variable
		"SETGLOBAL",  // set global variable 
		"GETUPVALUE",	// get upvalue
		"SETUPVALUE",	// set upvalue
		"CLOSURE",	// closure
		"CALL",		// call
		"RETURN",		// return
		"JMP",		// jump
		"MOVE",		// move
		"TEST",		// test
		"LOAD",	 	// load const value to register
		"INC",		// +=
		"ADD",	 	// +
		"SUB",	 	// -
		"MUL",		// *
		"DIV", 		// /
		"OR",			// |
		"AND",		// &
		"BIG",		// >
		"LITE",		// <
		"EQU",		// == 
		"NEQU",		// !=
		"BIGE",		// >=
		"LITEE",		// <=
		"NOT"		// !
	};

	int op = ir_gop(_ir);
	assert(op>0 && op<(sizeof(_irs)/sizeof(char*)));

	char* ops = _irs[op];
	int a = ir_ga(_ir);
	switch(ir_gmodle(_ir)){
		case ABC_MODE:{
			int b = ir_gb(_ir);
			int c = ir_gc(_ir);
			char b_buf[32] = {0};
			char* bs = (b<0)?(obj2str(closure_at_cstack(cls, b), b_buf, sizeof(b_buf)-1)):(NULL);
			char c_buf[32] = {0};
			char* cs = (c<0)?(obj2str(closure_at_cstack(cls, c), c_buf, sizeof(c_buf)-1)):(NULL);
			
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
			char*  bxs = (bx<0)?(obj2str(closure_at_cstack(cls, bx), bx_buf, sizeof(bx_buf)-1)):(NULL);
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
	dump_closure(curr_cls);
}





