#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"
#include <string.h>
 
// the key word
static char* _key[] = {
	"function", "return", "continue", "for", "if",
	"else", "foreach", "break", "nil", "in", NULL
};

#define MAX_IDE_TEMP_BUF 128

struct a2_lex{
	char* lex_map[32];
};

static void _init_lex(struct a2_lex* lex_p);

struct a2_lex* a2_lex_open(){
	struct a2_lex* ret = NULL;
	ret = (struct a2_lex*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));
	_init_lex(ret);
	return ret;
}


void a2_lex_close(struct a2_lex* lex_p){
	if(lex_p==NULL)
		return;
	free(lex_p);
}

inline  size_t _lex_hash(char* s){
	size_t len = strlen(s);
	return s[0] + s[len-1];
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