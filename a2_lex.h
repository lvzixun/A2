#ifndef _A2_LEX_H_
#define _A2_LEX_H_

#include "a2_conf.h"

enum {
	tk_key,			// keyWord     like: function if else ..
	tk_ide,			// identifier   var
	tk_string,		// string       'abcdefg'
	tk_number,		// number       12345
	tk_note,		// note :       # des
	tk_op, 			// operate      like: + - * / ( ) [ ]
	tk_args,		// variable parameters ...
	tk_strcat 		// string aappend ..
};

struct a2_token{
	uint32 tt;
	union{
		a2_number number;
		char* str;
	}v;
};

struct a2_lex;

struct a2_lex* a2_lex_open();
void a2_lex_close(struct a2_lex* lex_p);
void a2_lex_clear(struct a2_lex* lex_p);

// source's analysised and get an token list, you can get token list's len from token_len
struct a2_token* a2_lex_read(struct a2_lex* lex_p, struct a2_io* io_p, size_t* token_len);

#endif
