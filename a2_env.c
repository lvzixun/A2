#include "a2_conf.h"
#include "a2_map.h"
#include "a2_lex.h"
#include "a2_string.h"
#include "a2_gc.h"

struct a2_env{
	struct a2_map* g_str;			// global string map
	struct a2_lex* lex_p;
	struct a2_gc*  gc_p;

	// private forge a2_obj
	struct a2_obj _forge_obj;
	struct a2_gcobj* _forge_gcobj;			
};

struct a2_env* a2_env_new(){
	struct a2_env* ret = (struct a2_env*)malloc(sizeof(*ret));
	ret->g_str = a2_map_new();
	ret->lex_p = a2_lex_open(ret);
	ret->gc_p = a2_gc_new();
	ret->_forge_gcobj = a2_nil2gcobj();
	ret->_forge_obj.type = A2_TSNIL;
	return ret;
}

void a2_env_free(struct a2_env* env_p){
	assert(env_p);
	a2_map_free(env_p->g_str);
	a2_lex_close(env_p->lex_p);
	a2_gc_free(env_p->gc_p);
	a2_gcobj_nilfree(env_p->_forge_gcobj);
	free(env_p);
}

static inline struct a2_obj* _fill_str2obj(struct a2_env* env_p, char* a2_s){
	env_p->_forge_obj.type = A2_TSTRING;
	env_p->_forge_obj.value.obj = env_p->_forge_gcobj;
	a2_gcobj_setstring(env_p->_forge_gcobj, a2_s);
	return &(env_p->_forge_obj);
}

char* a2_env_addstr(struct a2_env* env_p, char* a2_s){
	assert(a2_s);
	assert(env_p);

	struct a2_obj* vp=a2_map_query(env_p->g_str, _fill_str2obj(env_p, a2_s));
	if(vp==NULL){
		struct a2_obj k, v;
		v.type = A2_TPOINT;
		struct a2_kv kv = {
			&k, &v
		};
		a2_s = a2_string_new(a2_s);
		k = a2_string2obj(a2_s);
		v.value.point = a2_s;
		a2_gc_add(env_p->gc_p, k.value.obj);		// add gc chain
		a2_map_add(env_p->g_str, &kv);				// add global string map
		return a2_s;
	}else
		return  (char*)(vp->value.point);
}