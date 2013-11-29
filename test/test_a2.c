
#include "../src/a2.h"
#include "../src/a2_libutil.h"
#include <stdio.h>
#include <string.h>

static void _load(struct a2_state* state, const char* str){
	if(a2_dostring(state, str, strlen(str))){
		printf("%s\n", a2_tostring(state, a2_top(state)-1));
	}
}

int main(int argc, char const *argv[])
{
	struct a2_state* as = a2_open();
	a2_openutil(as);

	_load(as, "a = 1123\nprint(a) eve('print(777777)')");
	_load(as, "b = 4456\nprint(b) eve('print(8888888)')");
/*
	a2_pushstring(as, "func");
	a2_getglobal(as);
	// a2_pushnumber(as, 1);
	a2_pushstring(as, "seek");
	if(a2_pcall(as, 1)){
		printf("%s\n", a2_tostring(as, a2_top(as)-1));
	}else{
		printf("ret = %lf\n", a2_tonumber(as, a2_top(as)-1));
	}
*/
	a2_close(as);
	mem_print();
	return 0;
}
