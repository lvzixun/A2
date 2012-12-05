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
	int32 tt;
	union{
		a2_number number;
		char* str;
	}v;
};

#endif