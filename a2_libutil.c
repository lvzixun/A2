#include "a2.h"
#include <stdio.h>

struct kf{
	char* key;
	a2_cfunction func;
};


int a2_libprint(struct a2_state* state);

void a2_openutil(struct a2_state* state){
	struct kf _reg_func[] = {
		{"print", a2_libprint}
	};
	int i;
	for(i=0; i<sizeof(_reg_func)/sizeof(struct kf); i++){
		a2_reg(state, _reg_func[i].key, _reg_func[i].func);
	}
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