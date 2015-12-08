#include "a2_conf.h"
#include "a2_map.h"
#include "a2_lex.h"
#include "a2_string.h"
#include "a2_gc.h"
#include "a2_gc.h"
#include "a2_io.h"
#include "a2_parse.h"
#include "a2_ir.h"
#include "a2_closure.h"
#include "a2_vm.h"
#include "a2.h"

#include "a2_error.h"

#include <stdio.h>


struct a2_env{
	struct a2_lex* lex_p;
	struct a2_gc*  gc_p;
	struct a2_parse* parse_p;
	struct a2_ir* ir_p;
	struct a2_vm* vm_p;
	
	// state
	struct a2_state* state;
	
	// global string map
	struct a2_map* g_str;			
	// global table
	struct a2_map* g_var;
	// register table
	struct a2_map* reg_var;

	// c <->a2 stack
	struct obj_stack cstack;
	// stack bottom
	int bottom;

	// expection 
	struct a2_longjump* jp;
	panic_func panic;


	// private forge a2_obj
	struct a2_obj _forge_obj;
	struct a2_gcobj* _forge_gcobj;	

	// meta key chain
	struct a2_obj  meta_keys[MK_COUNT];		
};

static void _a2_init_mk(struct a2_env* env_p);
static struct a2_gcobj* _a2_env_addstrobj(struct a2_env* env_p, char* a2_s, int is_copy);
static inline struct a2_obj* _set_envvar(struct a2_env* env_p, struct a2_map* g_map, struct a2_obj* k, struct a2_obj* v);

struct a2_env* a2_env_new(struct a2_state* state){
	struct a2_env* ret = (struct a2_env*)malloc(sizeof(*ret));
	ret->g_str = a2_map_new();
	ret->g_var = a2_map_new();
	ret->reg_var = a2_map_new();
	ret->state = state;
	ret->bottom = 0;
	ret->jp = NULL;
	obj_stack_init(&ret->cstack);
	ret->lex_p = a2_lex_open(ret);
	ret->parse_p = a2_parse_open(ret);
	ret->gc_p = a2_gc_new(ret);
	ret->vm_p = a2_vm_new(ret);
	ret->ir_p = a2_ir_open(ret);
	ret->_forge_gcobj = a2_nil2gcobj();
	_a2_init_mk(ret);
	return ret;
}

void a2_env_free(struct a2_env* env_p){
	assert(env_p);
	obj_stack_destory(&env_p->cstack);
	a2_lex_close(env_p->lex_p);
	a2_parse_close(env_p->parse_p);
	a2_gc_free(env_p->gc_p);
	a2_ir_close(env_p->ir_p);
	a2_vm_free(env_p->vm_p);
	a2_map_free(env_p->g_str);
	a2_map_free(env_p->g_var);
	a2_map_free(env_p->reg_var);
	a2_gcobj_nilfree(env_p->_forge_gcobj);
	free(env_p);
}

struct _a2_ld_args{
	struct a2_io* stream;
	struct a2_xclosure** xcls_p;
};

static void _a2_load(struct a2_env* env_p, struct _a2_ld_args* ud){
	size_t len = 0;
	a2_ir_newxcls(env_p->ir_p);
	struct a2_token* tk = a2_lex_read(env_p->lex_p, ud->stream, &len);
	
	#ifdef _DEBUG_
	printf("---------token --------------\n");
	int i;
	for (i = 0; i < len; ++i){
		print_token(&tk[i]);
	}
	printf("\n");
	#endif

	*(ud->xcls_p) = a2_parse_run(env_p->parse_p, tk, len);
	
	#ifdef _DEBUG_	
	dump_xclosure(env_p->ir_p, *(ud->xcls_p));
	#endif
}

static void _a2_init_mk(struct a2_env* env_p){
	char* mk_str[] = {
		"__index",
		"__newindex",
		"__gc",
		"__call"
	};

	assert(sizeof(mk_str)/sizeof(mk_str[0]) == MK_COUNT);
	int i;
	for(i=0; i<MK_COUNT; i++){
		struct a2_obj mk_obj = a2_env_addstr(env_p, mk_str[i]);
		struct a2_gcobj* _gcobj = obj_vX(&mk_obj, obj);
		a2_gc_mark(_gcobj, mark_blue);
		env_p->meta_keys[i] = mk_obj;
	}
}

struct a2_obj* a2_env_getmk(struct a2_env* env_p, enum mk t){
	assert(t<MK_COUNT && t>=0);
	return &(env_p->meta_keys[t]);
}

int a2_env_load(struct a2_env* env_p, struct a2_io* stream){
	struct a2_xclosure* xcls = NULL;
	struct _a2_ld_args args = {
		stream, &xcls
	};
	a2_gc_close(env_p->gc_p);
	int ret = a2_xpcall(env_p, (a2_pfunc)_a2_load, &args);
	
	a2_lex_clear(env_p->lex_p);
	a2_parse_clear(env_p->parse_p);
	a2_ir_clear(env_p->ir_p);

	if(ret == 0){
		struct a2_closure* cls = a2_closure_newrun(xcls);
		struct a2_gcobj* gcobj = a2_closure2gcobj(cls);
		struct a2_obj obj;
		obj_setX(&obj, A2_TCLOSURE, obj, gcobj);
		a2_gc_markit(env_p->gc_p, &obj, mark_blue);
		a2_gc_open(env_p->gc_p);
		a2_gcadd(env_p, gcobj); 

		// load it
		ret = a2_vm_load(env_p->vm_p, cls);
		a2_gc_markit(env_p->gc_p, &obj, mark_white);
	}else{
		a2_gc_open(env_p->gc_p);
	}

	return ret;
}

inline struct a2_state* a2_env2state(struct a2_env* env_p){
	return env_p->state;
}

static inline struct a2_obj* _fill_str2obj(struct a2_env* env_p, char* a2_s){
	obj_setX(&(env_p->_forge_obj), A2_TSTRING, obj, env_p->_forge_gcobj);
	a2_gcobj_setstring(env_p->_forge_gcobj, a2_s);
	return &(env_p->_forge_obj);
}

inline struct  a2_gcobj* a2_env_addstrobj(struct a2_env* env_p, char* a2_s){
	return _a2_env_addstrobj(env_p, a2_s, 1);
}

static struct a2_gcobj* _a2_env_addstrobj(struct a2_env* env_p, char* a2_s, int is_copy){
	assert(a2_s);
	assert(env_p);
	
	struct a2_obj* vp=a2_map_query(env_p->g_str, _fill_str2obj(env_p, a2_s));
	if(vp==NULL){
		struct a2_obj k, v;
		struct a2_kv kv = {
			&k, &v
		};
		if(is_copy)
			a2_s = a2_string_new(a2_s);
		k = a2_string2obj(a2_s);
		obj_setX(&v, A2_TPOINT, point, obj_vX(&k, obj));
		a2_gc_add(env_p->gc_p, obj_vX(&k, obj));		// add gc chain
		a2_map_add(env_p->g_str, &kv);				// add global string map
		return obj_vX(&k, obj);
	}else
		return  obj_vX(vp, point);
}

inline struct a2_obj a2_env_addstr(struct a2_env* env_p, char* str){
	char* a2_s = a2_string_new(str);
	struct a2_gcobj* _gcp = _a2_env_addstrobj(env_p, a2_s, 0);
	struct a2_obj ret;
	obj_setX(&ret, A2_TSTRING, obj, _gcp);
	return ret;
}

static inline void _env_gcmark(struct a2_env* env_p){
	struct a2_obj k = a2_nil2obj();
	struct a2_obj* vp = NULL;

	// mark global var
	while( NULL != (vp=a2_map_next(env_p->g_var, &k))){
		a2_gc_markit(env_p->gc_p, vp, mark_black);
		a2_gc_markit(env_p->gc_p, &k, mark_black);
	}

	// mark  register var
	k = a2_nil2obj();
	while(NULL != (vp=a2_map_next(env_p->reg_var, &k))){
		a2_gc_markit(env_p->gc_p, vp, mark_black);
		a2_gc_markit(env_p->gc_p, &k, mark_black);	
	}

	// mark eval stack
	int i;
	for(i=0; i<env_p->cstack.top; i++){
		struct a2_obj* obj = &env_p->cstack.stk_p[i];
		a2_gc_markit(env_p->gc_p, obj, mark_black);
	}
}

inline void a2_pushstack(struct a2_env* env_p, struct a2_obj* v){
	obj_stack_add(&env_p->cstack, v);
}

inline void a2_setbottom(struct a2_env* env_p, int bottom){
	assert(bottom>=0 && bottom<=env_p->cstack.top);
	env_p->bottom = bottom;
}

inline void a2_settop(struct a2_env* env_p, int top){
	assert(top>=0 && top>=env_p->bottom);
	env_p->cstack.top = top;
}

inline int a2_getbottom(struct a2_env* env_p){
	return env_p->bottom;
}

inline int a2_gettop(struct a2_env* env_p){
	return env_p->cstack.top;
}

inline struct a2_obj* a2_getcstack(struct a2_env* env_p, int idx){
	assert(idx>=0 && (idx+env_p->bottom)<env_p->cstack.top);
	return &(env_p->cstack.stk_p[idx+env_p->bottom]);
}

inline void a2_setcstack(struct a2_env* env_p, int idx, struct a2_obj* obj){
	assert(env_p);
	assert(obj);
	assert(idx>=0 && (idx+env_p->bottom)<env_p->cstack.top);
	env_p->cstack.stk_p[idx+env_p->bottom] = *obj;
}

inline struct a2_obj* a2_getcstk_top(struct a2_env* env_p){
	assert(env_p);
	assert(env_p->cstack.top>0);
	return &(env_p->cstack.stk_p[env_p->cstack.top-1]);
}

inline void a2_vmgc(struct a2_env* env_p){
	_env_gcmark(env_p);
	a2_vm_gc(env_p->vm_p);
}

inline struct a2_obj* a2_get_envglobal(struct a2_env* env_p, struct a2_obj* k){
	return a2_map_query(env_p->g_var, k);
}

inline struct a2_obj* a2_set_envglobal(struct a2_env *env_p, struct a2_obj* k, struct a2_obj* v){
	return _set_envvar(env_p, env_p->g_var, k, v);
}

inline struct a2_obj* a2_get_envreg(struct a2_env* env_p, struct a2_obj* k){
	return a2_map_query(env_p->reg_var, k);
}

inline struct a2_obj* a2_set_envreg(struct a2_env* env_p, struct a2_obj* k, struct a2_obj* v){
	return _set_envvar(env_p, env_p->reg_var, k, v);
}

static inline struct a2_obj* _set_envvar(struct a2_env* env_p, struct a2_map* g_map, struct a2_obj* k, struct a2_obj* v){
	struct a2_obj* ret = a2_map_query(g_map, k);

	if(ret==NULL){
		struct a2_kv kv = {
			k, v
		};
		a2_map_add(g_map, &kv);
		return v;
	}

	*ret = *v;
	return v;
}

inline void a2_irexec(struct a2_env* env_p, size_t root){
	a2_ir_exec(env_p->ir_p, root);
}

inline struct a2_xclosure* a2_irexend(struct a2_env* env_p){
	return a2_ir_exend(env_p->ir_p);
}

inline struct a2_node* a2_nodep(struct a2_env* env_p, size_t idx){
	return a2_node_p(env_p->parse_p, idx);
}

inline  void a2_gcadd(struct a2_env* env_p, struct a2_gcobj* gcobj){
	a2_gc_add(env_p->gc_p, gcobj);
}

inline struct a2_obj* a2_sfidx(struct a2_env* env_p, size_t sf_idx){
	return vm_sf_index(env_p->vm_p, sf_idx);
}

// key check function
inline int a2_ktisfunction(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisfunction(env_p->lex_p, token);
}

inline int a2_ktisreturn(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisreturn(env_p->lex_p, token);
}

inline int a2_ktiscontinue(struct a2_env* env_p, struct a2_token* token){
	return a2_tokeniscontinue(env_p->lex_p, token);
}

inline int a2_ktisfor(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisfor(env_p->lex_p, token);
}

inline int a2_ktisif(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisif(env_p->lex_p, token);
}

inline int a2_ktiselse(struct a2_env* env_p, struct a2_token* token){
	return a2_tokeniselse(env_p->lex_p, token);
}

inline int a2_ktisforeach(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisforeach(env_p->lex_p, token);
}

inline int a2_ktisbreak(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisbreak(env_p->lex_p, token);
}

inline int a2_ktisnil(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisnil(env_p->lex_p, token);
}

inline int a2_ktisin(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenisin(env_p->lex_p, token);
}

inline int a2_ktislocal(struct a2_env* env_p, struct a2_token* token){
	return a2_tokenislocal(env_p->lex_p, token);
}

void a2_env_clear_itstring(struct a2_env* env_p, struct a2_gcobj* str_gcobj){
	struct a2_obj k;
	obj_setX(&k, A2_TSTRING, obj, str_gcobj);
	a2_map_del(env_p->g_str, &k);
}

inline struct a2_lex* a2_envlex(struct a2_env* env_p){
	return env_p->lex_p;
}

inline struct a2_parse* a2_envparse(struct a2_env* env_p){
	return env_p->parse_p;
}

inline struct a2_ir* a2_envir(struct a2_env* env_p){
	return env_p->ir_p;
}

inline struct a2_gc* a2_envgc(struct a2_env* env_p){
	return env_p->gc_p;
}

inline struct a2_longjump** a2_envjp(struct a2_env* env_p){
	return &(env_p->jp);
}

inline struct a2_vm* a2_envvm(struct a2_env* env_p){
	return env_p->vm_p;
}

inline panic_func a2_envpanic(struct a2_env* env_p){
	return env_p->panic;
}

inline void a2_setpanic(struct a2_env* env_p, panic_func panic){
	env_p->panic = panic;
}

inline int a2_callpanic(struct a2_env* env_p){
	if(env_p->panic == NULL)
		return a2_fail;
	else
		env_p->panic(env_p->state);
	return a2_true;
}

