#ifndef _A2_PARSE_H_
#define _A2_PARSE_H_
#include "a2_lex.h"

struct a2_parse;

struct a2_parse*  a2_parse_open(struct a2_env* env_p);
void a2_parse_close(struct a2_parse* parse_p);
void a2_parse_run(struct a2_parse* parse_p, struct a2_token* token_chain, size_t len);
void a2_parse_clear(struct a2_parse* parse_p);

#endif
