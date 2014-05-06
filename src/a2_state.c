#include "a2.h"
#include "a2_env.h"
#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_gc.h"
#include "a2_error.h"
#include "a2_map.h"
#include "a2_array.h"
#include "a2_io.h"
#include "a2_error.h"
#include "a2_vm.h"
#include "a2_string.h"

#include <string.h>

struct a2_state{
	struct a2_env* env_p;
};

A2_API struct a2_state* a2_open(){
	struct a2_state* ret = (struct a2_state*)malloc(sizeof(*ret));
	ret->env_p = a2_env_new(ret);
	return ret;
}

A2_API void a2_close(struct a2_state* state){
	a2_env_free(state->env_p);
	free(state);
}

A2_API inline void a2_pop(struct a2_state* state, int count){
	int top = a2_gettop(state->env_p);
	int bottom = a2_getbottom(state->env_p);
	if(count > 0 && top-count>=bottom)
		a2_settop(state->env_p, top-count);
}


A2_API int a2_loadfile(struct a2_state* state, const char* file){
	struct a2_io* io_p = a2_io_open(state->env_p, file);
	int ret = a2_env_load(state->env_p,io_p);
	a2_io_close(io_p);
	return ret;
}

A2_API void a2_setmeta(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* m = a2_getcstack(state->env_p, top);
	struct a2_obj* v = a2_getcstack(state->env_p, top-1);

	if(obj_t(v)!=A2_TMAP || obj_t(m)!=A2_TMAP)
		a2_err(state, "the value and meta should map type.");

	a2_gc_setmeta(obj_vX(v, obj), obj_vX(m, obj));
	a2_topset(state, top-1);
}

A2_API int a2_pcall(struct a2_state* state, int args){
	struct a2_obj* cls_obj = a2_getcstack(state->env_p, a2_top(state)-args-1);
	struct a2_obj* args_obj = a2_getcstack(state->env_p, a2_top(state)-args);

	if(obj_t(cls_obj) !=A2_TCLOSURE)
		a2_error(state->env_p, e_run_error, "the type is not closure at pcall.\n");

	return a2_vm_pcall(a2_envvm(state->env_p), cls_obj, args_obj, args);
}

A2_API int a2_dostring(struct a2_state* state, const char* str, size_t len){
	if(str==NULL || len == 0)
		return 0;
	struct a2_io* io_p = a2_io_openS(str, len);
	int ret = a2_env_load(state->env_p,io_p);
	a2_io_closeS(io_p);
	return ret;
}

A2_API void a2_err(struct a2_state* state, const char* f, ...){
	va_list args;
	va_start(args, f);
	a2_vserror(state->env_p, e_run_error, f, &args);
}

A2_API inline int a2_type(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	return obj_t(obj);
}

A2_API inline void a2_len(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	size_t len =0;
	struct a2_obj len_obj;
	switch(obj_t(obj)){
		case A2_TMAP:
			len = a2_map_len(a2_gcobj2map(obj_vX(obj, obj)));
			break;
		case A2_TARRAY:
			len = a2_array_len(a2_gcobj2array(obj_vX(obj, obj)));
			break;
		case A2_TSTRING:
			len = a2_string_len(a2_gcobj2string(obj_vX(obj, obj)));
			break;
		default:
			a2_error(state->env_p, e_run_error,
			 "the type is not map or array at len function.");
	}
	len_obj = a2_number2obj((a2_number)len);
	a2_pushstack(state->env_p, &len_obj);
}	

A2_API inline const char* a2_typeinfo(struct a2_state* state, int idx){
	return a2_type2string(a2_type(state, idx));
}

A2_API inline  void a2_pushnumber(struct a2_state* state, a2_number number){
	struct a2_obj obj = a2_number2obj(number);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline a2_number a2_tonumber(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj_t(obj)!=A2_TNUMBER)
		return 0.0;
	else 
		return obj_vNum(obj);
}

A2_API inline void* a2_topoint(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	switch(obj_t(obj)){
		case A2_TARRAY:
			return a2_gcobj2array(obj_vX(obj, obj));
		case A2_TMAP:
			return a2_gcobj2map(obj_vX(obj, obj));
		case A2_TCLOSURE:
			return a2_gcobj2closure(obj_vX(obj, obj));
		default:
			return NULL;			
	}
}

A2_API inline void a2_pushnil(struct a2_state* state){
	struct a2_obj obj = a2_nil2obj();
	a2_pushstack(state->env_p, &obj);
}

A2_API inline void a2_pushstring(struct a2_state* state, char* str){
	struct a2_obj obj = a2_env_addstr(state->env_p, str);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline char* a2_tostring(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj_t(obj)!=A2_TSTRING)
		return "";
	else
		return  a2_gcobj2string(obj_vX(obj, obj));
}

A2_API inline void a2_pushbool(struct a2_state* state, int b){
	struct a2_obj obj = a2_bool2obj(b);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline int a2_tobool(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj_t(obj)!=A2_TBOOL)
		a2_error(state->env_p, e_run_error, 
			"the type is not bool.");
	else
		return  (int)(obj_vX(obj, uinteger));
	return 0;
}

A2_API inline void a2_pushfunction(struct a2_state* state, a2_cfunction func){
	struct a2_obj obj = a2_cfunction2obj(func);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline a2_cfunction a2_tocfunction(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj_t(obj)!=A2_TCFUNCTION)
		return NULL;
	else
		return obj_vX(obj, cfunction);
}

A2_API inline void a2_pushvalue(struct a2_state* state, int idx){
	struct a2_obj* _v = a2_getcstack(state->env_p, idx);
	a2_pushstack(state->env_p, _v);
}

A2_API inline void a2_setvalue(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstk_top(state->env_p);
	a2_setcstack(state->env_p, idx, obj);
}


A2_API inline void a2_reg(struct a2_state* state, char* func_name, a2_cfunction func){
	assert(func_name && func);
	a2_pushstring(state, func_name);
	a2_pushfunction(state, func);
	a2_setglobal(state);
}


#define check_num(num)	if(obj_t(num)!=A2_TNUMBER) \
							a2_error(state->env_p, e_run_error, "the key is not number.");

#define check_key(k)	if(obj_t(k)!=A2_TSTRING && obj_t(k)!=A2_TNUMBER) \
							a2_error(state->env_p, e_run_error, "the key is not number or string.\n"); 

#define check_map(map)  if(obj_t(map)!=A2_TMAP) \
							a2_error(state->env_p, e_run_error, "the varable is not map type."); 

#define check_array(array)	if(obj_t(array)!=A2_TARRAY) \
							 a2_error(state->env_p, e_run_error, "the varable is not array type.");

// get global
A2_API inline void a2_getglobal(struct a2_state* state){
	struct a2_obj* k = a2_getcstack(state->env_p, a2_top(state)-1);
	check_key(k);
	struct a2_obj* v = a2_get_envglobal(state->env_p, k);
	if(v==NULL)
		obj_setX(k, A2_TNIL, point, NULL);
	else
		*k = *v;
}

// set global
A2_API inline void a2_setglobal(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* v = a2_getcstack(state->env_p, top);
	struct a2_obj* k = a2_getcstack(state->env_p, top-1);

	check_key(k);
	a2_set_envglobal(state->env_p, k, v);
	a2_topset(state, top-1);
}

A2_API inline void a2_require(struct a2_state* state){
	struct a2_obj* k = a2_getcstack(state->env_p, a2_top(state)-1);
	check_key(k);
	struct a2_obj* v = a2_get_envreg(state->env_p, k);
	if(v) // return obj
		a2_pushstack(state->env_p, v);
	else{ // load obj
		const char* name = a2_gcobj2string(obj_vX(k, obj));
		int len = strlen(name);
		char tmp[len+8];
		memcpy(tmp, name, len);
		memcpy(tmp+len, ".a2", 4);
		int top = a2_top(state);
		a2_loadfile(state, tmp);
		if(a2_top(state)>top)
			a2_pushvalue(state, top);
		else
			a2_pushnil(state);
		a2_set_envreg(state->env_p, k, a2_getcstack(state->env_p, top));
	}
}

// set map
A2_API inline void a2_setmap(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* v = a2_getcstack(state->env_p, top);
	struct a2_obj* k = a2_getcstack(state->env_p, top-1);
	struct a2_obj* map = a2_getcstack(state->env_p, top-2);

	check_map(map);
	check_key(k);
	a2_map_set(a2_gcobj2map(obj_vX(map, obj)), k, v);
	a2_pop(state, 1);
}


// get map
A2_API inline void a2_getmap(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* k = a2_getcstack(state->env_p, top);
	struct a2_obj* map = a2_getcstack(state->env_p, top-1);
	struct a2_obj* v = NULL;

	check_map(map);
	check_key(k);
	v = a2_map_query(a2_gcobj2map(obj_vX(map, obj)), k);
	if(v==NULL)
		obj_setX(map, A2_TNIL, point, NULL);
	else
		*map = *v;
}

// del key/value
A2_API inline void a2_delmap(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* k = a2_getcstack(state->env_p, top);
	struct a2_obj* map = a2_getcstack(state->env_p, top-1);

	check_map(map);
	check_key(k);
	a2_map_del(a2_gcobj2map(obj_vX(map, obj)), k);
	a2_pop(state, 1);
}

// set array
A2_API inline void a2_setarray(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* v = a2_getcstack(state->env_p, top);
	struct a2_obj* k = a2_getcstack(state->env_p, top-1);
	struct a2_obj* array = a2_getcstack(state->env_p, top-2);
	struct a2_obj* _v = NULL;

	check_array(array);
	check_num(k);
	_v  = a2_array_get(a2_gcobj2array(obj_vX(array, obj)), k);
	if(_v==NULL)
		a2_error(state->env_p, e_run_error, 
			"the error index at array.\n");
	else
		*_v = *v;
	a2_topset(state, top-1);
}


// get array
A2_API inline void a2_getarray(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* k = a2_getcstack(state->env_p, top);
	struct a2_obj* array = a2_getcstack(state->env_p, top-1);
	struct a2_obj* _v = NULL;

	check_array(array);
	check_num(k);
	_v = a2_array_get(a2_gcobj2array(obj_vX(array, obj)), k);
	if(_v==NULL)
		obj_setX(k, A2_TNIL, point, NULL);
	else
		*k = *_v;
}

// add array
A2_API inline void a2_addarray(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* v = a2_getcstack(state->env_p, top);
	struct a2_obj* array = a2_getcstack(state->env_p, top-1);

	check_array(array);
	a2_array_add(a2_gcobj2array(obj_vX(array, obj)), v);
}

A2_API inline int a2_top(struct a2_state* state){
	return a2_gettop(state->env_p)-a2_getbottom(state->env_p);
}

A2_API inline void a2_topset(struct  a2_state* state, int idx){
	a2_settop(state->env_p, a2_getbottom(state->env_p)+idx);
}




