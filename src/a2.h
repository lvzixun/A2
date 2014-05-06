#ifndef  _A2_H_
#define  _A2_H_
#include "a2_conf.h"

struct a2_state;

// type

#define 	TNIL 1
#define 	TSTRING 2
#define 	TNUMBER 3
#define 	TPOINT 4
#define 	TMAP 5
#define 	TARRAY 6
#define 	TCLOSURE 7
#define 	TCFUNCTION 8
#define 	TBOOL 9

typedef int (*a2_cfunction)(struct a2_state*);
A2_API struct a2_state* a2_open();
A2_API void a2_close(struct a2_state* state);	
A2_API int  a2_loadfile(struct a2_state* state, const char* file);
A2_API int a2_dostring(struct a2_state* state, const char* str, size_t len);
A2_API void a2_err(struct a2_state* state, const char* f, ...);
A2_API int a2_pcall(struct a2_state* state, int args);

// a2 <-> c stack
A2_API inline void a2_pop(struct a2_state* state, int count);
A2_API inline int a2_top(struct a2_state* state);
A2_API inline void a2_topset(struct  a2_state* state, int idx);

A2_API inline const char* a2_typeinfo(struct a2_state* state, int idx);
A2_API inline int a2_type(struct a2_state* state, int idx);
A2_API inline void a2_len(struct a2_state* state, int idx);
A2_API inline void* a2_topoint(struct a2_state* state, int idx);

A2_API inline int a2_tobool(struct a2_state* state, int idx);
A2_API inline void a2_pushbool(struct a2_state* state, int b);
A2_API inline  void a2_pushnumber(struct a2_state* state, a2_number number);
A2_API inline a2_number a2_tonumber(struct a2_state* state, int idx);
A2_API inline void a2_pushstring(struct a2_state* state, char* str);
A2_API inline char* a2_tostring(struct a2_state* state, int idx);
A2_API inline void a2_pushfunction(struct a2_state* state, a2_cfunction func);
A2_API inline a2_cfunction a2_tocfunction(struct a2_state* state, int idx);
A2_API inline void a2_pushvalue(struct a2_state* state, int idx);
A2_API inline void a2_setvalue(struct a2_state* state, int idx);
A2_API inline void a2_reg(struct a2_state* state, char* func_name, a2_cfunction func);
A2_API inline void a2_getglobal(struct a2_state* state);
A2_API inline void a2_setglobal(struct a2_state* state);
A2_API inline void a2_setmap(struct a2_state* state);
A2_API inline void a2_getmap(struct a2_state* state);
A2_API inline void a2_delmap(struct a2_state* state);
A2_API inline void a2_setarray(struct a2_state* state);
A2_API inline void a2_getarray(struct a2_state* state);
A2_API inline void a2_addarray(struct a2_state* state);

A2_API inline void a2_require(struct a2_state* state);

A2_API void a2_setmeta(struct a2_state* state);
#endif
