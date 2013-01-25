#ifndef _A2_ERROR_H_
#define _A2_ERROR_H_
#include <stdarg.h>

void a2_error(const char* f, ...);
void a2_vserror(const char* f, va_list* args);
#endif