#include "a2_conf.h"
#include "a2_lex.h"
#include "a2_env.h"
#include "a2_error.h"

struct a2_node{
	byte type;
	struct a2_token* token;
	size_t childs[4];
	size_t next;
};

struct a2_parse{
	struct a2_env* env_p;
	struct a2_token* token_chain;
	size_t len;
	size_t t_idx;

	// node buf
	byte level;
	size_t cap;
	struct a2_node* node_buf;

	size_t node_head;
};

#define DEFAULT_NODE_LEVEL	6
#define _node_deep(l)	(1<<(l))
static void _init_node(struct a2_parse* parse_p);


#define cur_token	(parse_p->token_chain[parse_p->t_idx])
static inline struct a2_token* parse_attoken(struct a2_parse* parse_p);
static inline struct a2_token* parse_readtoken(struct a2_parse* parse_p);
static inline struct a2_token* parse_matchtoken(struct a2_parse* parse_p, size_t n);
static  void parse_segment(struct a2_parse* parse_p);
static  void parse_expression(struct a2_parse* parse_p);
static  void parse_function(struct a2_parse* parse_p);
static  void parse_for(struct a2_parse* parse_p);
static  void parse_foreach(struct a2_parse* parse_p);
static  void parse_if(struct a2_parse* parse_p);
static void parse_operation(struct a2_parse* parse_p);
static void parse_opstring(struct a2_parse* parse_p);

struct a2_parse*  a2_parse_open(struct a2_env* env_p){
	struct a2_parse* ret = (struct a2_parse*)malloc(sizeof(*ret));
	assert(env_p);
	ret->env_p = env_p;
	ret->cap=0;
	_init_node(ret);
	return ret;
}


void a2_parse_close(struct a2_parse* parse_p){
	assert(parse_p);
	free(parse_p->node_buf);
	free(parse_p);
}

void a2_parse_clear(struct a2_parse* parse_p){
	assert(parse_p);
	parse_p->token_chain = NULL;
	parse_p->t_idx = parse_p->len = 0;
}

// parse token chain
void a2_parse_run(struct a2_parse* parse_p, struct a2_token* token_chain, size_t len){
	assert(parse_p);
	assert(token_chain);
	assert(len);

	parse_p->len = len;
	parse_p->token_chain = token_chain;
	parse_p->t_idx = 0;

	parse_segment(parse_p);
}

// node function
static void _init_node(struct a2_parse* parse_p){
	parse_p->node_buf = (struct a2_node*)malloc(sizeof(struct a2_node)*_node_deep(DEFAULT_NODE_LEVEL));
	parse_p->level = DEFAULT_NODE_LEVEL;
}

static inline void _resize_node(struct a2_parse* parse_p){
	parse_p->node_buf = (struct a2_node*)realloc(parse_p->node_buf, sizeof(struct a2_node)*_node_deep(parse_p->level+1));
	parse_p->level++;
}

static inline size_t get_node(struct a2_parse* parse_p){
	if(parse_p->cap>=_node_deep(parse_p->level))
		_resize_node(parse_p);

	return parse_p->cap++;
}

static inline void clear_node(struct a2_parse* parse_p){
	parse_p->cap = 0;
}

#define node_p(i)	(&(parse_p->node_buf[i]))


#define CHECK_END(n)	do{if(parse_p->t_idx+(n) >= parse_p->len)return NULL;}while(0)
static inline struct a2_token* parse_attoken(struct a2_parse* parse_p){
	CHECK_END(0);
	return &(parse_p->token_chain[parse_p->t_idx]);
}

static inline struct a2_token* parse_readtoken(struct a2_parse* parse_p){
	CHECK_END(0);
	return &(parse_p->token_chain[(parse_p->t_idx)++]);
}

// 
static inline struct a2_token* parse_matchtoken(struct a2_parse* parse_p, size_t n){
	CHECK_END(n);
	return &(parse_p->token_chain[parse_p->t_idx+n]);
}

// parse a segment
static void parse_segment(struct a2_parse* parse_p){
	for( ; parse_p->t_idx<parse_p->len; ){
		switch( tt2tk(cur_token.tt) ){
			case tk_end:   // 
				(parse_p->t_idx)++;
				break;
			case tk_key:
				if(a2_ktisfunction(parse_p->env_p, &cur_token) == a2_true){ //  parse function
					parse_function(parse_p);
				}else if(a2_ktisfor(parse_p->env_p, &cur_token) == a2_true){ // parse for
					parse_for(parse_p);
				}else if(a2_ktisforeach(parse_p->env_p, &cur_token) == a2_true){ // parse foreach
					parse_foreach(parse_p);
				}else if(a2_ktisfor(parse_p->env_p, &cur_token) == a2_true){ // parse for
					parse_for(parse_p);
				}else if(a2_ktisreturn(parse_p->env_p, &cur_token) == a2_true){ // parse return

				}else if(a2_ktisif(parse_p->env_p, &cur_token) == a2_true){	// parse if
					parse_if(parse_p);
				}else{	// error token
					goto ERROR_TOKEN;
				}
				break;
			case tk_op:
				switch(tt2op(cur_token.tt)){
					case '(':
					case '-':
					case '+':
						parse_expression(parse_p);
						break;
					default:
						goto ERROR_TOKEN; 	
				}
				break;	
			case tk_string:
			case tk_number:
			case tk_ide:
				parse_expression(parse_p);
				break;
			default:
				goto ERROR_TOKEN;
		}
	}

	return;
ERROR_TOKEN:{
	char ts_buf[64] = {0};
	a2_error("[parse error@line: %d]: the token \'%s \'is not allow.", 
		cur_token.line, 
		a2_token2str(&(cur_token), ts_buf));
	}
}

#define parse_error(s)	a2_error("[parse error@line: %d]: %s", cur_token.line, (s))

static  void parse_expression(struct a2_parse* parse_p){
	switch(tt2tk(cur_token.tt)){
		case tk_ide:{
			struct a2_token* tp = parse_matchtoken(parse_p, 1);
			if(tp == NULL)
				parse_error("expression is invalid.");
			switch(tt2tk(tp->tt)){
				case tk_op:
					// TODO: op
					parse_operation(parse_p);
					break;
				default:{
					char ts_buf[64] = {0};
					a2_error("[parse error@line: %d]: at token \'%s\' is error grammar.",
						tp->line, a2_token2str(tp, ts_buf));
				}
				break;
			}
		}
			break;
		case tk_string:{
			parse_opstring(parse_p);
		}	
			break;
		case tk_number:
		case tk_op:{
			parse_operation(parse_p);
		}
			break;
		default:
			assert(0);
	}
}

// parse 
static void parse_operation(struct a2_parse* parse_p){
	
}

static void parse_opstring(struct a2_parse* parse_p){

}

static  void parse_function(struct a2_parse* parse_p){

}

static  void parse_for(struct a2_parse* parse_p){

}

static  void parse_foreach(struct a2_parse* parse_p){

}

static  void parse_if(struct a2_parse* parse_p){

}

