#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"
#include "a2_obj.h"

#include <setjmp.h>

struct a2_longjump{
	jmp_buf buf;
	int status;
	struct a2_longjump* next;
};

void a2_error(struct a2_env* env_p, enum error_code ec, const char* f, ...){
	va_list args;
	va_start(args, f);
	a2_vserror(env_p, ec, f, &args);
}

void a2_vserror(struct a2_env* env_p, enum error_code ec, const char* f, va_list* args){
	char buf[128] = {'\0'};
	assert(ec!=e_no_error);

	vsnprintf(buf, sizeof(buf)-1, f, *args);
	va_end(*args);
	
	#ifdef _DEBUG_
	printf("|find error:| %s", buf);
	#endif
	
	struct a2_longjump** curr_jpp = a2_envjp(env_p);
	if(*curr_jpp == NULL || is_intererror(ec)){
		a2_callpanic(env_p); // call panic
		exit(0);
	}else{
		(*curr_jpp)->status = ec;
		struct a2_obj obj = a2_env_addstr(env_p, buf);
		a2_pushstack(env_p, &obj);
		longjmp((*curr_jpp)->buf, 1);
	}
}

int a2_xpcall(struct a2_env* env_p, a2_pfunc pfunc, void* ud){
	assert(pfunc);
	struct a2_longjump longjump;
	struct a2_longjump** curr_jpp = a2_envjp(env_p);
	longjump.status = 0;
	longjump.next = *curr_jpp;
	*curr_jpp = &longjump;

	if(setjmp(longjump.buf) == 0){
		pfunc(env_p, ud);
	}
	*curr_jpp = longjump.next;
	return longjump.status;
}

