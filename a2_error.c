#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"

void a2_error(const char* f, ...){
	va_list args;
	va_start(args, f);
	a2_vserror(f, &args);
}

void a2_vserror(const char* f, va_list* args){
	char buf[128] = {'\0'};
	
	
	vsprintf(buf, f, *args);
	va_end(*args);

	printf("%s", buf);
	exit(0);
}
