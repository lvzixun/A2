#include "a2.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

struct kf{
	char* key;
	a2_cfunction func;
};


int a2_libprint(struct a2_state* state);
int a2_libadd(struct a2_state* state);
int a2_libt_time(struct a2_state* state);
int a2_libsleep(struct a2_state* state);

void a2_openutil(struct a2_state* state){
	struct kf _reg_func[] = {
		{"print", a2_libprint},
		{"add", a2_libadd},
		{"t_time", a2_libt_time},
		{"sleep", a2_libsleep}
	};
	
	int i;
	for(i=0; i<sizeof(_reg_func)/sizeof(struct kf); i++){
		a2_reg(state, _reg_func[i].key, _reg_func[i].func);
	}
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
				a2_err(state, "the number of args is error at map set.[exp: add(map, key value)]");
			a2_setmap(state);
			break;
		default:
			a2_err(state, "the varable is not container.");
	}
	return 0;
}


// t_time
int a2_libt_time(struct a2_state* state){
	struct timeval st;
 	gettimeofday(&st, 0);
 	
 	a2_pushnumber(state, (a2_number)(st.tv_sec*1000000+st.tv_usec));
 	return 1;
}

// sleep
int a2_libsleep(struct a2_state* state){
	if(a2_type(state, 0)!=TNUMBER)
		a2_err(state, "the arg must number.");
	a2_number t = a2_tonumber(state, 0);
	sleep(t);
	return 0;
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
			default:
				assert(0);
		}
	}
	
	printf("\n");
	return 0;
}