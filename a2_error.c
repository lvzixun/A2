#include <stdio.h>
#include <stdarg.h>

void a2_error(const char* f, ...){
	va_list args;
	va_start(args, f);
	printf(f, args);
	va_end(args);
}
