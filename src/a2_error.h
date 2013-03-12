#ifndef _A2_ERROR_H_
#define _A2_ERROR_H_
#include <stdarg.h>
#include "a2_env.h"

#define inter_error 0x0100

enum error_code{
	e_no_error,

	e_run_error,
	e_io_error,
	e_lex_error,
	e_parse_error,
	e_ir_error,
	e_vm_error,

	// internal error
	e_mem_error = inter_error+1,
	e_overfllow_error = inter_error+2
};

#define is_intererror(ec)	((ec) & (inter_error))

struct a2_longjump;

typedef void(*a2_pfunc)(struct a2_env* , void*);
int a2_xpcall(struct a2_env* env_p, a2_pfunc pfunc, void* ud);

void a2_error(struct a2_env* env_p, enum error_code ec, const char* f, ...);
void a2_vserror(struct a2_env* env_p, enum error_code ec, const char* f, va_list* args);
#endif

