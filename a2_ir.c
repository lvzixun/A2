#include "a2_conf.h"
#include "a2_env.h"
#include "a2_lex.h"
#include "a2_error.h"
#include "a2_parse.h"
#include "a2_map.h"
#include "a2_ir.h"
#include "a2_gc.h"
#include "a2_closure.h"
#include <stdio.h>

#define node_p(i) 	  a2_nodep(ir_p->env_p, (i))
#define node_t(i)	  (node_p(i)->type)
#define node_ct(n,i)  (node_p(node_p(n)->childs[i])->type)
#define node_cp(n,i)  (node_p(node_p(n)->childs[i]))

#define ir_error(i, s)  do{a2_error("[ir error@line: %lu]: %s\n",node_p(i)->token->line,s);}while(0)
#define curr_sym (ir_p->cls_sym_chain->sym.sym_chain[ir_p->cls_sym_chain->sym.cap-1])
#define curr_csym (ir_p->cls_sym_chain->sym.sym_chain[0])
#define curr_cls (ir_p->cls_sym_chain->cls)
#define curr_clssym (ir_p->cls_sym_chain)
#define add_arg  _add_arg(ir_p)
#define curr_arg (curr_clssym->arg_cap)
#define set_arg(i) _set_arg(ir_p,i)

#define top_arg (assert(curr_clssym->arg_cap), curr_clssym->arg_cap-1)
#define del_arg  _del_arg(ir_p)

#define sym_v(vt,i)	(((vt)<<(sizeof(uint32)-2))|(i))
#define v2vt(v)  ((v)>>(sizeof(uint32)-2)) 
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

struct cls_sym{
	struct symbol  sym;
	struct a2_closure* cls;
	int arg_cap;
	int max_arg;
	byte is_recycle;
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

static inline void new_symbol(struct a2_ir* ir_p);
static inline void free_symbol(struct a2_ir* ir_p);
static inline void _a2_ir_exec(struct a2_ir* ir_p, struct cls_sym* cls_sp, size_t root);

static struct cls_sym* cls_sym_new();
static void cls_sym_free(struct cls_sym* p);

static inline void  new_clssym(struct a2_ir* ir_p);
static inline void free_clssym(struct a2_ir* ir_p);

static inline struct a2_obj node2obj(struct a2_ir* ir_p, size_t node);

static int a2_ir_ass(struct a2_ir* ir_p, size_t root);
static void a2_ir_local(struct a2_ir* ir_p, size_t root);
static int a2_ir_exp(struct a2_ir* ir_p, size_t root);
static inline int a2_ir_var(struct a2_ir* ir_p, size_t root);

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

static struct cls_sym* cls_sym_new(){
	struct cls_sym* ret = (struct cls_sym*)malloc(sizeof(*ret));
	ret->next = NULL;
	ret->cls = a2_closure_new();

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
		case  ass_node:		// 
			a2_ir_ass(ir_p, root);
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


static inline void  new_clssym(struct a2_ir* ir_p){
	struct cls_sym* np = cls_sym_new();
	// add closure to gc chain
	a2_gcadd(ir_p->env_p,  a2_closure2gcobj(np->cls));
	
	np->next = ir_p->cls_sym_chain;
	ir_p->cls_sym_chain = np;
	new_symbol(ir_p); // const symbol
	new_symbol(ir_p); // local symbol
}

static inline void free_clssym(struct a2_ir* ir_p){
	assert(ir_p->cls_sym_chain);
	struct cls_sym* np = ir_p->cls_sym_chain->next;
	cls_sym_free(ir_p->cls_sym_chain);
	ir_p->cls_sym_chain = np;
}

// add const varable symbol
static inline int  add_csymbol(struct a2_ir* ir_p, struct a2_obj* k){
	assert(k->type==A2_TNUMBER || k->type==A2_TSTRING);
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

// add local varable symbol
static inline int add_lsymbol(struct a2_ir* ir_p, struct a2_obj* k, size_t root){
	assert(k->type==A2_TSTRING);
	struct a2_obj v = a2_uinteger2obj(sym_v(var_local, curr_clssym->arg_cap));
	curr_clssym->arg_cap++;
	struct a2_kv kv = {
		k, &v
	};
	if(a2_map_add(curr_sym, &kv) == a2_fail){
		ir_error(root, "the varable is volatile.");
	}
	return curr_clssym->arg_cap;
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
			*vt_p = v2vt(vp->value.uinteger);
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
					int idx = closure_push_upvalue(curr_cls, p->cls, v2i(vp->value.uinteger));
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

static void a2_ir_local(struct a2_ir* ir_p, size_t root){
	assert(node_t(root)==local_node);
	size_t node = node_p(root)->next;
	size_t n, b;
	struct a2_obj k;
	while(node){
		n = node_p(node)->next;
		switch(node_t(node)){
			case var_node:
				b = node;
				goto LOCAL_VAR;
			case ass_node:{
				assert(node_p(node_p(node)->childs[0])->type==var_node);
				b = node_p(node)->childs[0];
LOCAL_VAR:		
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
				if(curr_clssym->is_recycle){
					closure_add_ir(curr_cls, ir_abc(SETGLOBAL, r_idx, top_arg, r_idx));
					del_arg;
				}else{
					if(is_Climit(r_idx)){
						int ki=top_arg;
						closure_add_ir(curr_cls, ir_abx(LOAD, add_arg, r_idx));
						closure_add_ir(curr_cls, ir_abc(SETGLOBAL, ki, ki, top_arg));
						del_arg;
					}else{
						closure_add_ir(curr_cls, ir_abc(SETGLOBAL, top_arg, top_arg, r_idx));
					}
					curr_clssym->is_recycle=1;
				}
				return top_arg; 
			}else{
				switch(vt){
					case var_local:
						assert(idx>0);
						closure_add_ir(curr_cls, ir_abx(LOAD, idx-1, r_idx));
						if(curr_clssym->is_recycle)
							del_arg;
						return idx-1;
					case var_upvalue:
						closure_add_ir(curr_cls, ir_abx(SETUPVALUE, idx-1, r_idx));
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
			curr_clssym->is_recycle = 1;
			return _b;
		case var_node:
			return a2_ir_var(ir_p, root);
		case num_node:{
			struct a2_obj k = a2_number2obj(node_p(root)->token->v.number);
			return add_csymbol(ir_p, &k);
		}
		case str_node:{
			struct a2_obj k = {
				A2_TSTRING, node_p(root)->token->v
			};
			return add_csymbol(ir_p, &k);
		}
			break;
		case ass_node:
			return a2_ir_ass(ir_p, root);
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
		curr_clssym->is_recycle = 1;
		return top_arg;
	}else{
		switch(vt){
			case var_global:
				closure_add_ir(curr_cls, ir_abx(GETGLOBAL, add_arg, idx));
				curr_clssym->is_recycle = 1;
				return top_arg;
			case var_local:
				curr_clssym->is_recycle = 0;
				return idx-1;
			case var_upvalue:
				closure_add_ir(curr_cls, ir_abx(GETUPVALUE, add_arg, idx-1));
				curr_clssym->is_recycle = 1;
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
		"LOAD",	 	// load const value to register
		"ADD",	 	// +
		"SUB",	 	// -
		"MUL",		// *
		"DIV" 		// /
	};

	int op = ir_gop(_ir);
	assert(op>0 && op<ir_count);

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





