#include <stdio.h>
#include "a2_error.h"
#include <string.h>
#include "a2_io.h"
#include "a2_lex.h"
 
// the key word
static char* _key[] = {
	"function", "return", "continue", "for", "if",
	"else", "foreach", "break", "nil", "in", NULL
};

#define DEFAULT_TOKENS_LEVEL 5
#define DEFAULT_TOKENS_LEN	(1<<DEFAULT_TOKENS_LEVEL)
#define _deep(l) (((size_t)1)<<(l))

struct _tokens{
	size_t len;
	byte level;
	struct a2_token token_p[1];
};

struct a2_lex{
	char* lex_map[32];
	struct _tokens* ts_p;			// tokens buffer
};

static void _init_lex(struct a2_lex* lex_p);
static struct _tokens* _lex_resize(struct _tokens* ts_p);
inline void _lex_addtoken(struct _tokens* ts_p, struct a2_token* token);


struct a2_lex* a2_lex_open(){
	struct a2_lex* ret = NULL;
	ret = (struct a2_lex*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));
	_init_lex(ret);
	ret->ts_p = _lex_resize(NULL);
	return ret;
}

void a2_lex_close(struct a2_lex* lex_p){
	if(lex_p==NULL) return;
	free(lex_p->ts_p);
	free(lex_p);
}

struct a2_token* a2_lex_read(struct a2_lex* lex_p, struct a2_io* io_p, size_t* token_len){
	check_null(lex_p, NULL);
	assert(io_p);
	assert(token_len);

	// set return
	*token_len = lex_p->ts_p->len;
	return lex_p->ts_p->token_p;
}

void a2_lex_clear(struct a2_lex* lex_p){
	if(!lex_p) return;
	lex_p->ts_p->len = 0;
}

inline  size_t _lex_hash(char* s){
	return s[0] + s[strlen(s)-1];
}

static void _init_lex(struct a2_lex* lex_p){
	int i=0;
	while(_key[i]){
		size_t idx = _lex_hash(_key[i])%32;
		a2_assert(!lex_p->lex_map[idx]);
		lex_p->lex_map[idx] = _key[i];
		i++;
	}
}

static struct _tokens* _lex_resize(struct _tokens* ts_p){
	struct _tokens* ret = NULL;
	if(!ts_p){
		ret = (struct _tokens*)malloc(sizeof(*ret)+ sizeof(struct a2_token)*DEFAULT_TOKENS_LEN);
		ret->len=0;
		ret->level = DEFAULT_TOKENS_LEVEL;
	}else{
		ts_p->level++;
		ret = (struct _tokens*)realloc(ts_p, _deep(ts_p->level)*sizeof(struct a2_token));
	}
	return ret;
}

inline void _lex_addtoken(struct _tokens* ts_p, struct a2_token* token){
	if(ts_p->len>=_deep(ts_p->level))
		ts_p = _lex_resize(ts_p);

	ts_p->token_p[ts_p->len++] = *token;
}
