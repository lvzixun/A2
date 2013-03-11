#ifndef _A2_LEX_H_
#define _A2_LEX_H_

#include "a2_conf.h"
#include "a2_env.h"
#include "a2_obj.h"

enum {
	tk_key,			// keyWord     like: function if else ..
	tk_ide,			// identifier   var
	tk_string,		// string       'abcdefg'
	tk_number,		// number       12345
	tk_end,			//  /n:       	
	tk_op, 			// operate      like: + - * / ( ) [ ]
	tk_bool,		// bool true/false
	tk_nil 			// nil
};

struct a2_lex;
struct a2_io;

struct a2_token{
	uint32 tt;
	size_t line;	
	a2_value v;
};

#define tt2tk(tt)	((tt)>>24)
#define tt2op(tt)	((tt)&0xffffff)
#define kp2tt(k, p) (((k)<<24) | (p)) 
inline uint32 tk_mask(byte op, const char* s);

struct a2_lex* a2_lex_open(struct a2_env* env_p);
void a2_lex_close(struct a2_lex* lex_p);
void a2_lex_clear(struct a2_lex* lex_p);

// source's analysised and get an token list, you can get token list's len from token_len
struct a2_token* a2_lex_read(struct a2_lex* lex_p, struct a2_io* io_p, size_t* token_len);

// token operation
char* a2_token2str(struct a2_token* token, char* ts_buf);
// key token operation
inline int a2_tokenisfunction(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisreturn(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokeniscontinue(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisfor(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisif(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokeniselse(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisforeach(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisbreak(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisnil(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenisin(struct a2_lex* lex_p, struct a2_token* token);
inline int a2_tokenislocal(struct a2_lex* lex_p, struct a2_token* token);

// for test
void print_token(struct a2_token* token);

#endif
