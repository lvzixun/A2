#include <stdio.h>
#include "a2_conf.h"
#include <stdarg.h>

void a2_error(char* f, ...){
	char buf[128] = {'\0'};
	va_list args;
	va_start(args, f);
	vsprintf(buf, f, args);
	va_end(args);

	printf("%s", buf);
	exit(0);
}
