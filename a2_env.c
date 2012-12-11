#include "a2_conf.h"
#include "a2_map.h"
#include "a2_lex.h"
#include "a2_string.h"
#include "a2_gc.h"

struct a2_env{
	struct a2_map* g_str;			// global string map
	struct a2_lex* lex_p;
	struct a2_gc*  gc_p;			
};

struct a2_env* a2_env_new(){
	struct a2_env* ret = (struct a2_env*)malloc(sizeof(*ret));
	ret->g_str = a2_map_new();
	ret->lex_p = a2_lex_open(ret);
	ret->gc_p = a2_gc_new();
	return ret;
}

void a2_env_free(struct a2_env* env_p){
	assert(env_p);
	a2_map_free(env_p->g_str);
	a2_lex_close(env_p->lex_p);
	a2_gc_free(env_p->gc_p);
	free(env_p);
}

char* a2_env_addstr(struct a2_env* env_p, char* a2_s){
	assert(a2_s);
	assert(env_p);
	struct a2_obj k = a2_string2obj(a2_s);
	struct a2_obj v;
	v.type = A2_TSNIL;
	struct a2_kv kv = {
		&k, &v
	};
	
	struct a2_obj* vp=a2_map_query(env_p->g_str, &k);
	if(vp==NULL){
		a2_s = a2_string_new(a2_s);
		k = a2_string2obj(a2_s);
		a2_gc_add(env_p->gc_p, k.value.obj);		// add gc chain
		a2_map_add(env_p->g_str, &kv);				// add global string map
		return a2_s;
	}else
		return  a2_gcobj2string(vp->value.obj);
}