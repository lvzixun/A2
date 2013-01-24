#include "a2.h"
#include "a2_env.h"
#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_gc.h"
#include "a2_error.h"
#include "a2_map.h"
#include "a2_array.h"
#include "a2_io.h"

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

A2_API void a2_loadfile(struct a2_state* state, const char* file){
	struct a2_io* io_p = a2_io_open(file);
	a2_env_load(state->env_p,io_p);
	a2_io_close(io_p);
}


A2_API inline int a2_type(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	return obj->type;
}

A2_API inline  void a2_pushnumber(struct a2_state* state, a2_number number){
	struct a2_obj obj = a2_number2obj(number);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline a2_number a2_tonumber(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj->type!=A2_TNUMBER)
		return 0.0;
	else 
		return obj->value.number;
}

A2_API inline void a2_pushstring(struct a2_state* state, char* str){
	struct a2_obj obj = a2_env_addstr(state->env_p, str);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline char* a2_tostring(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj->type!=A2_TSTRING)
		return "";
	else
		return  a2_gcobj2string(obj->value.obj);
}

A2_API inline void a2_pushbool(struct a2_state* state, int b){
	struct a2_obj obj = a2_bool2obj(b);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline int a2_tobool(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj->type!=A2_TBOOL)
		a2_error("the type is not bool.");
	else
		return  (int)(obj->value.uinteger);
	return 0;
}

A2_API inline void a2_pushfunction(struct a2_state* state, a2_cfunction func){
	struct a2_obj obj = a2_cfunction2obj(func);
	a2_pushstack(state->env_p, &obj);
}

A2_API inline a2_cfunction a2_tocfunction(struct a2_state* state, int idx){
	struct a2_obj* obj = a2_getcstack(state->env_p, idx);
	if(obj->type!=A2_TCFUNCTION)
		return NULL;
	else
		return obj->value.cfunction;
}

A2_API inline void a2_pushvalue(struct a2_state* state, int idx){
	struct a2_obj* _v = a2_getcstack(state->env_p, idx);
	a2_pushstack(state->env_p, _v);
}


A2_API inline void a2_reg(struct a2_state* state, char* func_name, a2_cfunction func){
	assert(func_name && func);
	a2_pushstring(state, func_name);
	a2_pushfunction(state, func);
	a2_setglobal(state);
}


#define check_num(num)	if((num)->type!=A2_TNUMBER) \
							a2_error("the key is not number.");

#define check_key(k)	if((k)->type!=A2_TSTRING && (k)->type!=A2_TNUMBER) \
							a2_error("the key is not number or string.\n"); 

#define check_map(map)  if((map)->type!=A2_TMAP) \
							a2_error("the varable is not map type."); 

#define check_array(array)	if((array)->type!=A2_TARRAY) \
							 a2_error("the varable is not array type.");

// get global
A2_API inline void a2_getglobal(struct a2_state* state){
	struct a2_obj* k = a2_getcstack(state->env_p, a2_gettop(state->env_p)-1);
	check_key(k);
	struct a2_obj* v = a2_get_envglobal(state->env_p, k);
	if(v==NULL)
		k->type=A2_TNIL;
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

// set map
A2_API inline void a2_setmap(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* v = a2_getcstack(state->env_p, top);
	struct a2_obj* k = a2_getcstack(state->env_p, top-1);
	struct a2_obj* map = a2_getcstack(state->env_p, top-2);

	check_map(map);
	check_key(k);
	a2_map_set(a2_gcobj2map(map->value.obj), k, v);
	a2_topset(state, top-1);
}


// get map
A2_API inline void a2_getmap(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* k = a2_getcstack(state->env_p, top);
	struct a2_obj* map = a2_getcstack(state->env_p, top-1);
	struct a2_obj* v = NULL;

	check_map(map);
	check_key(k);
	v = a2_map_query(a2_gcobj2map(map->value.obj), k);
	if(v==NULL)
		map->type = A2_TNIL;
	else
		*map = *v;
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
	_v  = a2_array_get(a2_gcobj2array(array->value.obj), k);
	if(_v==NULL)
		a2_error("the error index at array.\n");
	*_v = *v;
	a2_topset(state, top-1);
}


// get array
A2_API inline void a2_getarray(struct a2_state* state){
	int top = a2_top(state)-1;
	struct a2_obj* k = a2_getcstack(state->env_p, top-1);
	struct a2_obj* array = a2_getcstack(state->env_p, top-2);
	struct a2_obj* _v = NULL;

	check_array(array);
	check_num(k);
	_v = a2_array_get(a2_gcobj2array(array->value.obj), k);
	if(_v==NULL)
		k->type = A2_TNIL;
	else
		*k = *_v;
}


A2_API inline int a2_top(struct a2_state* state){
	return a2_gettop(state->env_p)-a2_getbottom(state->env_p);
}

A2_API inline void a2_topset(struct  a2_state* state, int idx){
	a2_settop(state->env_p, a2_getbottom(state->env_p)+idx);
}




