#include <stdio.h>
#include "a2_error.h"
#include <string.h>
#include "a2_io.h"
#include "a2_lex.h"
#include "a2_string.h"
 
// the key word
static char* _key[] = {
	"function", "return", "continue", "for", "if",
	"else", "foreach", "break", "nil", "in", "true",
	"false", "local", NULL
};

static char cmask[] = {
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\t', '\n', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', ' ', '!', '\"', '#', '\0', '\0', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '0', '0', 
	'0', '0', '0', '0', '0', '0', '0', '\0', ';', '<', '=', '>', '\0', '\0', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', '[', '\0', ']', '\0', 'A', '\0', 'A', 'A', 'A', 'A', 'A', 
	'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', '{', '|', '}', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};


#define DEFAULT_TOKENS_LEN	128
#define lex_error(s) a2_error(lex_p->env_p, e_lex_error, "[lex error @line: %zd]:%s\n", lex_p->line, s)
#define _mask(c)	(cmask[(uchar)(c)])
//#define tk_mask(tk, v)	 ((((uint32)tk)<<24)|(v))


#define LEX_MAP_DEEP 32
struct a2_lex{
	struct a2_env* env_p;
	char* lex_map[LEX_MAP_DEEP];
	byte  lex_str2hash[sizeof(_key)/sizeof(char*)];
	size_t line;
	char*  a2_s_num_bufs;
	struct{
		size_t cap;
		size_t size;
		struct a2_token* token_p;
	}ts;
};

static void _init_lex(struct a2_lex* lex_p);
inline void _lex_addtoken(struct a2_lex* lex_p, struct a2_token* token);
static inline void lex_string(struct a2_lex* lex_p, struct a2_io* io_p);
static inline void lex_number(struct a2_lex* lex_p, struct a2_io* io_p);
static inline void lex_identifier(struct a2_lex* lex_p, struct a2_io* io_p);
static inline a2_number _hex2number(char* a2_s);
static inline int _is_key(struct a2_lex* lex_p, char* s);
static char* op2s(char* ops, uint32 op);

struct a2_lex* a2_lex_open(struct a2_env* env_p){
	struct a2_lex* ret = NULL;
	ret = (struct a2_lex*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));
	_init_lex(ret);
	ret->env_p = env_p;

	ret->ts.cap = 0;
	ret->ts.size = DEFAULT_TOKENS_LEN;
	ret->ts.token_p = (struct a2_token*)malloc(sizeof(struct a2_token)*DEFAULT_TOKENS_LEN);
	ret->a2_s_num_bufs = a2_str_new;
	return ret;
}

void a2_lex_close(struct a2_lex* lex_p){
	if(lex_p==NULL) return;
	free(lex_p->ts.token_p);
	a2_string_free(lex_p->a2_s_num_bufs);
	free(lex_p);
}

struct a2_token* a2_lex_read(struct a2_lex* lex_p, struct a2_io* io_p, size_t* token_len){
	char c;
	check_null(lex_p, NULL);
	assert(io_p);
	assert(token_len);

	lex_p->line = 1;

	while(!a2_io_end(io_p)){
		// printf("%c", a2_io_readchar(io_p));
		// continue;
		c = a2_io_atchar(io_p);
		switch( _mask(c) ){
			case '#':			// note
				for(;(c)&&c!='\n';c=a2_io_readchar(io_p)){ }
				if(c=='\n')
					(lex_p->line)++;
				break;
			case '\'':			// string
			case '\"':
				lex_string(lex_p, io_p);
				break;
			case 'A':			// Identifiers
				lex_identifier(lex_p, io_p);
				break;
			case '0':			// number
				lex_number(lex_p, io_p);
				break;
			case '.':{
				struct a2_token token;
				token.line = lex_p->line;
				a2_io_readchar(io_p);
				token.tt = tk_mask(tk_op, ".");			// op .
				if(a2_io_atchar(io_p)=='.'){
					token.tt = tk_mask(tk_op, "..");		// op .. string cat
					a2_io_readchar(io_p);
					if(a2_io_atchar(io_p)=='.'){			// op ... args
						token.tt = tk_mask(tk_op, "...");
						a2_io_readchar(io_p);
					}
				}
				_lex_addtoken(lex_p, &token);
			}
				break;
			case '{':
			case '}':
			case '(':
			case '[':
			case ']':
			case ')':
			case ',':			// op ,
			case '&':			// logic &
			case '|':			// logic |
			case '+':			// op +
			case '-':			// op -
			case '*':			// op *
			case '/':{			// op /
				char _ts[2] = {'\0', '\0'};
				struct a2_token token;
				token.line = lex_p->line;
				_ts[0] = c;
				token.tt = tk_mask(tk_op, _ts);
				_lex_addtoken(lex_p, &token);
				a2_io_readchar(io_p);
			}
				break;
			case '>':			// op > or >=
			case '<':			// op < or <=
			case '=':			// op = or ==
			case '!':{			// op ! or !=
				char _ts[3] = {'\0', '\0', '\0'};
				struct a2_token token;
				token.line = lex_p->line;
				_ts[0] = c;
				a2_io_readchar(io_p);
				if(a2_io_atchar(io_p)=='='){
					_ts[1] = '=';
					token.tt = tk_mask(tk_op, _ts);
					a2_io_readchar(io_p);
				}
				else
					token.tt = tk_mask(tk_op, _ts);
				_lex_addtoken(lex_p, &token);
			}	
				break;

			case '\n':			// next line
				(lex_p->line)++;
			case ';':{
				struct a2_token* tp = &(lex_p->ts.token_p[lex_p->ts.cap-1]);
					uint32 up_tt = tp->tt;
					if(lex_p->ts.cap>0 && tt2tk(up_tt) != tk_end 
					   && tt2tk(up_tt)!=tk_op && (tt2tk(up_tt)!=tk_key || a2_ktisreturn(lex_p->env_p, tp)==a2_true)){
						struct a2_token token;
						token.line = lex_p->line-1;
						token.tt = tk_mask(tk_end, 0);
						_lex_addtoken(lex_p, &token);
					}
				}
			case '\t':
			case ' ':			// jump
				a2_io_readchar(io_p);
				break;
			default:			// error
				a2_error(lex_p->env_p, e_lex_error, 
					"[lex error @line: %zd]: the character\' %c \' is not allow.\n",
				 	lex_p->line, c);
				break;
		}
	}

	// set return
	*token_len = lex_p->ts.cap;
	return lex_p->ts.token_p;
}

void a2_lex_clear(struct a2_lex* lex_p){
	if(!lex_p) return;
	lex_p->ts.cap = 0;
}

inline  size_t _lex_hash(char* s){
	size_t ret = ((s[0] + s[strlen(s)-1] + (s[0]<<8)) % 100);
	return ret%LEX_MAP_DEEP;
}

static void _init_lex(struct a2_lex* lex_p){
	int i=0;
	while(_key[i]){
		size_t idx = _lex_hash(_key[i]);
		lex_p->lex_str2hash[i] = (byte)idx;
		assert(!lex_p->lex_map[idx]);
		lex_p->lex_map[idx] = _key[i];
		i++;
	}
}


inline void _lex_addtoken(struct a2_lex* lex_p, struct a2_token* token){
	if(lex_p->ts.cap >= lex_p->ts.size){
		lex_p->ts.size *= 2;
		lex_p->ts.token_p = (struct a2_token*)realloc(lex_p->ts.token_p, 
			sizeof(struct a2_token)*lex_p->ts.size);
	}
	lex_p->ts.token_p[lex_p->ts.cap++] = *token;
}

// analysis string
static inline void lex_string(struct a2_lex* lex_p, struct a2_io* io_p){
	char c;
	a2_string_clear(lex_p->a2_s_num_bufs);
	struct a2_token  token;
	token.tt = tk_mask(tk_string, 0);
	token.line = lex_p->line;

	char b = a2_io_readchar(io_p);		// match '
	while(!a2_io_end(io_p)){
		c=a2_io_readchar(io_p);
		if(c=='\\'){ // escape char
			switch(a2_io_atchar(io_p)){
				case 't':
					c = '\t';
					a2_io_readchar(io_p);
					break;
				case 'n':
					c = '\n';
					a2_io_readchar(io_p);
					break;
				case  '\"':
					c = '\"';
					a2_io_readchar(io_p);
					break;
				case '\'':
					c = '\'';
					a2_io_readchar(io_p);
					break;
				case '\\':
					c = '\\';
					a2_io_readchar(io_p);
					break;
				default:
					c = '\\';
					break;
			}
			goto READ_C;
		}else if(c == '\n'){
			lex_p->line++;
		} else if( c!=b ){
READ_C:	
			lex_p->a2_s_num_bufs = a2_string_append(lex_p->a2_s_num_bufs, c);
		}else{
			token.v.obj = a2_env_addstrobj(lex_p->env_p, lex_p->a2_s_num_bufs);
			_lex_addtoken(lex_p, &token);
			return;
		}
	}

	lex_error("not match \"\'\"");
}

// analysis number
static inline void lex_number(struct a2_lex* lex_p, struct a2_io* io_p){
	char c, nc;
	struct a2_token token;
	a2_string_clear(lex_p->a2_s_num_bufs);
	token.tt = tk_mask(tk_number, 0);
	token.line = lex_p->line;
	token.v.number = (a2_number)0;

	c = a2_io_atchar(io_p);
	nc = a2_io_matchchar(io_p, 1);

	// HEX
	if(c=='0' && (nc=='x'|| nc=='X')){
		a2_io_readchar(io_p);
		a2_io_readchar(io_p);		// jump 0X
		while(!a2_io_end(io_p)){
			c=a2_io_atchar(io_p);
			if( _mask(c)=='0' || (_mask(c)=='A' && ((c>='A' && c<='F')||(c>='a' && c<='f'))) )
				lex_p->a2_s_num_bufs = a2_string_append(lex_p->a2_s_num_bufs, c);
			else
				break;
			a2_io_readchar(io_p);
		}
		token.v.number = _hex2number(lex_p->a2_s_num_bufs);
	}
	else{		// 10
		while(!a2_io_end(io_p)){
			c=a2_io_atchar(io_p);
			if( (c=='.' && _mask(a2_io_matchchar(io_p, 1))=='0') || _mask(c)=='0')
				lex_p->a2_s_num_bufs = a2_string_append(lex_p->a2_s_num_bufs, c);
			else if(c=='.' && a2_io_matchchar(io_p, 1)!='.')
				lex_error("the number is error, must number char after \' . \'.");
			else
				break;
			a2_io_readchar(io_p);
		}
		token.v.number = atonum(lex_p->a2_s_num_bufs);
	}

	_lex_addtoken(lex_p, &token);
}

// analysis identifier
static inline void lex_identifier(struct a2_lex* lex_p, struct a2_io* io_p){
	char c;
	a2_string_clear(lex_p->a2_s_num_bufs);
	struct a2_token token;
	token.line = lex_p->line;

	while(!a2_io_end(io_p)){
		c=a2_io_atchar(io_p);
		if(_mask(c)=='A'||_mask(c)=='0')
			lex_p->a2_s_num_bufs = a2_string_append(lex_p->a2_s_num_bufs, c);
		else
			break;
		a2_io_readchar(io_p);
	}

//	printf("ide = %s %s\n", token.v.str, (lex_p)->lex_map[_lex_hash(lex_p->a2_s_num_bufs)]);
	if(_is_key(lex_p, lex_p->a2_s_num_bufs)==a2_true){
		if(strcmp(lex_p->a2_s_num_bufs, "true")==0){
			token.tt = kp2tt(tk_bool, 1);
		}else if(strcmp(lex_p->a2_s_num_bufs, "false")==0){
			token.tt = kp2tt(tk_bool, 0);
		}else if(strcmp(lex_p->a2_s_num_bufs, "nil")==0){
			token.tt = kp2tt(tk_nil, 0);
		}else{
			token.tt = tk_mask(tk_key, 0);
			token.tt |= _lex_hash(lex_p->a2_s_num_bufs);
		}
	}
	else
		token.tt = tk_mask(tk_ide, 0);
	token.v.obj = a2_env_addstrobj(lex_p->env_p, lex_p->a2_s_num_bufs);
	_lex_addtoken(lex_p, &token);
}

static inline a2_number _hex2number(char* a2_s){
	a2_number ret = 0;
	char m = '\0';
	while(*a2_s){
		switch( _mask(*a2_s) ){
			case '0':
				m = *a2_s - '0';
				break;
			case 'A':
				m =  10 + *a2_s - ((*a2_s>='a' && *a2_s<='f')?('a'):('A'));
				break;
			default:
				assert(0);
				break;
		}

		ret = ret*16 + m;
		a2_s++;
	}
	return ret;
}

inline uint32 tk_mask(byte op, const char* s){
	uint32 ret=0;
	int i=0;
	for(i=0; s&&s[i]&&i<3; i++){
		ret <<=8;
		ret |=s[i];
	}
	return (ret) | (op<<24);
}

static inline int _is_key(struct a2_lex* lex_p, char* s){
	char* _s = lex_p->lex_map[_lex_hash(s)];
	
	return (_s && strcmp(_s, s)==0)?(a2_true):(a2_fail);
}

#define _token_check(i)		(token->tt == kp2tt(tk_key, lex_p->lex_str2hash[i]))?(a2_true):(a2_fail)
// check token is function
inline int a2_tokenisfunction(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(0);
}

inline int a2_tokenisreturn(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(1);
}

inline int a2_tokeniscontinue(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(2);
}

inline int a2_tokenisfor(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(3);
}

inline int a2_tokenisif(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(4);
}

inline int a2_tokeniselse(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(5);
}

inline int a2_tokenisforeach(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(6);
}

inline int a2_tokenisbreak(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(7);
}

inline int a2_tokenisnil(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(8);
}

inline int a2_tokenisin(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(9);
}

inline int a2_tokenistrue(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(10);
}

inline int a2_tokenisfalse(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(11);
}

inline int a2_tokenislocal(struct a2_lex* lex_p, struct a2_token* token){
	return _token_check(12);
}


char* a2_token2str(struct a2_token* token, char* ts_buf){
	if(!token) return "<null>";
	switch( tt2tk (token->tt) ){
		case tk_end:
			return "exp end";
		case tk_number:
			sprintf(ts_buf, "%lf", token->v.number);
			return ts_buf;
		case tk_key:
		case tk_ide:
		case tk_string:
			return a2_gcobj2string(token->v.obj);
		case tk_op:
			return op2s(ts_buf, tt2op(token->tt));
		case tk_bool:
			return (tt2op(token->tt))?("true"):("false");
		case tk_nil:
			return "nil";
		default:
			assert(0);
	}
	return  NULL;
}

void print_token(struct a2_token* token){
	if(!token)
		printf("<null>\n");
	else{
		char ts_buf[64] = {0};
		printf("%s\n", a2_token2str(token, ts_buf));
	}
}

static char* op2s(char* ops, uint32 op){
	int i=0;
	char c=0;
	c = (op&0xff0000)>>16;
	if(c)
		ops[i++]=c;
	c=(op&0xff00)>>8;
	if(c)
		ops[i++]=c;
	c=(op&0xff);
	if(c)
		ops[i++]=c;
	ops[i] = '\0';
	return ops;
}
