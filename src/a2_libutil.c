#include "a2.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include <stdlib.h>

struct kf{
	char* key;
	a2_cfunction func;
};


int a2_libprint(struct a2_state* state);
int a2_libadd(struct a2_state* state);
int a2_libt_time(struct a2_state* state);
int a2_libtype(struct a2_state* state);
int a2_liblen(struct a2_state* state);
int a2_libdostring(struct a2_state* state);
int a2_libkiss(struct a2_state* state);
int a2_libsystem(struct a2_state* state);
int a2_librandom(struct a2_state* state);
int a2_libdofile(struct a2_state* state);
int a2_librequire(struct a2_state* state);
int a2_libdel(struct a2_state* state);
int a2_libsetmeta(struct a2_state* state);

void a2_openutil(struct a2_state* state){
	struct kf _reg_func[] = {
		{"print", a2_libprint},
		{"add", a2_libadd},
		{"del", a2_libdel},
		{"t_time", a2_libt_time},
		{"type", a2_libtype},
		{"len", a2_liblen},
		{"eve", a2_libdostring},
		{"load", a2_libdofile},
		{"require", a2_librequire},
		{"random", a2_librandom},
		{"kiss", a2_libkiss},
		{"os", a2_libsystem},
		{"setmeta", a2_libsetmeta},
	};
	
	int i;
	for(i=0; i<sizeof(_reg_func)/sizeof(struct kf); i++){
		a2_reg(state, _reg_func[i].key, _reg_func[i].func);
	}
}

/*
int a2_libpcall(struct a2_state* state){
	int args = a2_top(state);
	if(args==0)
		a2_err(state, "the number of args is error.[exp: pcall(func, ...)]");
	if(a2_pcall(state, args-1)){
		a2_err(state, "%s", a2_tostring(state, a2_top(state)-1));
	}
	int _args = a2_top(state);
	return _args-args;
}
*/

int a2_libsetmeta(struct a2_state* state){
	int args = a2_top(state);
	if(args<2)
		a2_err(state, "the arg error.");
	a2_setmeta(state);
	return 0;
}

int a2_librequire(struct a2_state* state){
	int args = a2_top(state);
	if(args == 0 || a2_type(state, 0)!=TSTRING)
		a2_err(state, "the arg must string type.");
	a2_pushvalue(state, 0);  // set key
	a2_require(state);
	return 1;
}

int a2_librandom(struct a2_state* state){
	int args = a2_top(state);
	if(args == 0){
		a2_pushnumber(state, (a2_number)rand());
	}else{
		a2_number num = a2_tonumber(state, 0);
		a2_pushnumber(state, rand()%((int)num));
	}
	return 1;
}

int a2_libsystem(struct a2_state* state){
	int args = a2_top(state);
	if(args==0 || a2_type(state, 0)!=TSTRING)
		a2_err(state, "the arg must string type.");
	const char* str = a2_tostring(state, 0);
	system(str);
	return 0;
}

int a2_libkiss(struct a2_state* state){
	a2_pushstring(state, "\nalex-2 powered by zixun.\n\n:eve is not eve.\n:Ang, do not forget your dreams.\n");
	return 1;
}

// type
int a2_libtype(struct a2_state* state){
	int args = a2_top(state);
	if(args==0)
		a2_err(state, "the number of args is error.[exp: type(varable)]");
	a2_pushstring(state, (char*)(a2_typeinfo(state, 0)));
	a2_setvalue(state, 0);
	return 1;
}


int a2_liblen(struct a2_state* state){
	int args = a2_top(state);
	if(args==0)
		a2_err(state, "the number of args is error.[exp: len(varable)]");
	a2_len(state, 0);
	a2_setvalue(state, 0);
	return 1;
}

int a2_libdostring(struct a2_state* state){
	int args = a2_top(state);
	if(args==0 || a2_type(state, 0)!=TSTRING)
		a2_err(state, "the arg must string type.");
	const char* str = a2_tostring(state, 0);
	int top = a2_top(state);
	a2_dostring(state, str, strlen(str));
	return a2_top(state) - top;
}

int a2_libdofile(struct a2_state* state){
	int args = a2_top(state);
	if(args==0 || a2_type(state, 0)!=TSTRING)
		a2_err(state, "the arg must string type.");
	const char* filename = a2_tostring(state, 0);
	int top = a2_top(state);
	a2_loadfile(state, filename);
	return a2_top(state) - top;
}

// add
int a2_libadd(struct a2_state* state){
	int args = a2_top(state);
	if(args<2)
		a2_err(state, "the number of args is error.");
	switch(a2_type(state, 0)){
		case TARRAY:
			if(args!=2)
				a2_err(state, "the number of args is error at array add.[exp: add(array, value)]");
			a2_addarray(state);
			break;
		case TMAP:
			if(args!=3)
				a2_err(state, "the number of args is error at map set.[exp: add(map, key, value)]");
			a2_setmap(state);
			break;
		default:
			a2_err(state, "the varable is not container.");
	}
	return 0;
}

// del
int a2_libdel(struct a2_state* state){
	int args = a2_top(state);
	if(args==2 && a2_type(state, 0)==TMAP)
		a2_delmap(state);
	else
		a2_err(state, "exp: del(map, key)");
	return 0;
}

// t_time
int a2_libt_time(struct a2_state* state){
	struct timeval st;
 	gettimeofday(&st, 0);
 	
 	a2_pushnumber(state, (a2_number)((st.tv_sec*1000000+st.tv_usec)/1000000.0));
 	return 1;
}

// print 
int a2_libprint(struct a2_state* state){
	int args = a2_top(state);
	int i;
	for(i=0; i<args; i++){
		switch(a2_type(state, i)){
			case TNIL:
				printf("nil ");
				break;
			case TNUMBER:
				printf("%.14g ", a2_tonumber(state, i));
				break;
			case TSTRING:
				printf("%s ", a2_tostring(state, i));
				break;
			case TBOOL:
				printf("%s ", (a2_tobool(state, i))?("true"):("false"));
				break;
			case TCFUNCTION:
				printf("cfunc:%p ", a2_tocfunction(state, i));
				break;
			case TCLOSURE:
				printf("closure:%p ", a2_topoint(state, i));
				break;
			case TARRAY:
				printf("array:%p ", a2_topoint(state, i));
				break;
			case TMAP:
				printf("map:%p ", a2_topoint(state, i));
				break;
			default:
				assert(0);
		}
	}
	
	printf("\n");
	return 0;
}

