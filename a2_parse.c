#include "a2_conf.h"
#include "a2_lex.h"
#include "a2_env.h"
#include "a2_error.h"

enum ne{
	ne_right = 0,
	ne_left = 1
};

#define nt_lmask  ((ne_left)<<8)
#define nt2ne(nt)	((nt)>>8)

enum node_type{

	// string node
	strcat_node,// ..

	// arithmetic node
	add_node,	// +
	sub_node,	// -
	mul_node,	// *
	div_node,	// /

	// logic node
	and_node,	// &
	or_node,	// |
	not_node,	// !
	neg_node,   // -
	gt_node,	// >
	lt_node,	// <
	equ_node,	// ==
	gte_node, 	// >=
	lte_node,	// <=
	ne_node,	// !=

	if_node,	// if

	cfunc_node,	//  call  func()
	args_node,	// ...
	chi_node, 	// .
	func_node, 	// def function

	var_node = (nt_lmask | 0x00),  // variable node
	ass_node = (nt_lmask | 0x01),   // =
	cma_node = (nt_lmask | 0x02),	// map['key'] or  array[idx]
};

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
#define node_p(i)	(&(parse_p->node_buf[i-1]))
#define is_end		(parse_p->t_idx>=parse_p->len)	
#define parse_error(s)	a2_error("[parse error@line: %d]: %s", cur_token.line, (s))

static void _init_node(struct a2_parse* parse_p);


#define cur_token	(parse_p->token_chain[parse_p->t_idx])
static inline struct a2_token* parse_attoken(struct a2_parse* parse_p);
static inline struct a2_token* parse_readtoken(struct a2_parse* parse_p);
static inline struct a2_token* parse_matchtoken(struct a2_parse* parse_p, size_t n);


typedef size_t(*parse_func)(struct a2_parse*);
static void parse_gsegment(struct a2_parse* parse_p);
static size_t parse_lsegment(struct a2_parse* parse_p);
static   size_t _parse_expression(struct a2_parse* parse_p, parse_func pfunc);
static inline size_t parse_expression(struct a2_parse* parse_p);
static inline size_t parse_exp(struct a2_parse* parse_p);
static  size_t parse_function(struct a2_parse* parse_p);
static  size_t parse_for(struct a2_parse* parse_p);
static  size_t parse_foreach(struct a2_parse* parse_p);
static  size_t parse_if(struct a2_parse* parse_p);
static  size_t _parse_operation(struct a2_parse* parse_p, parse_func pfunc);
static  inline size_t parse_operation(struct a2_parse* parse_p);
static inline size_t parse_op(struct a2_parse* parse_p);
static  size_t parse_logic(struct a2_parse* parse_p);
static size_t parse_map(struct a2_parse* parse_p);
static size_t parse_array(struct a2_parse* parse_p);
static size_t parse_interval(struct a2_parse* parse_p);
static size_t parse_arithmetic(struct a2_parse* parse_p);
static size_t parse_advanced(struct a2_parse* parse_p);
static size_t parse_limits(struct a2_parse* parse_p);
static size_t parse_base(struct a2_parse* parse_p);
static size_t parse_args(struct a2_parse* parse_p);
static size_t parse_comma(struct a2_parse* parse_p);
static size_t parse_cargs(struct a2_parse* parse_p);
static size_t parse_child(struct a2_parse* parse_p);
static size_t parse_stringcats(struct a2_parse* parse_p);

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

	parse_gsegment(parse_p);
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

static inline size_t new_node(struct a2_parse* parse_p, struct a2_token* token,  enum node_type nt){
	size_t idx = get_node(parse_p);
	node_p(idx)->next=0;
	node_p(idx)->token = token;
	node_p(idx)->type = nt;
	return idx;
}

/*
static inline size_t new_node(struct a2_parse* parse_p, ){
	size_t idx = get_node(parse_p);

}*/

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

static size_t parse_segcontent(struct a2_parse* parse_p){
	switch( tt2tk(cur_token.tt) ){
		case tk_end:   // 
			(parse_p->t_idx)++;
			break;
		case tk_key:
			if(a2_ktisfunction(parse_p->env_p, &cur_token) == a2_true){ //  parse function
				return	parse_function(parse_p);
			}else if(a2_ktisfor(parse_p->env_p, &cur_token) == a2_true){ // parse for
				return 	parse_for(parse_p);
			}else if(a2_ktisforeach(parse_p->env_p, &cur_token) == a2_true){ // parse foreach
				return 	parse_foreach(parse_p);
			}else if(a2_ktisfor(parse_p->env_p, &cur_token) == a2_true){ // parse for
				return 	parse_for(parse_p);
			}else if(a2_ktisreturn(parse_p->env_p, &cur_token) == a2_true){ // parse return

			}else if(a2_ktisif(parse_p->env_p, &cur_token) == a2_true){	// parse if
				return 	parse_if(parse_p);
			}else{	// error token
				goto ERROR_TOKEN;
			}
			break;
		case tk_op:
			switch(tt2op(cur_token.tt)){
				case '(':
				case '-':
				case '+':
				case '{':
				case ']':
					return 	parse_expression(parse_p);
					break;
				default:
					goto ERROR_TOKEN; 	
			}
			break;	
		case tk_string:
		case tk_number:
		case tk_ide:
			return 	parse_expression(parse_p);
			break;
		default:
			goto ERROR_TOKEN;
	}

	return 0;

ERROR_TOKEN:{
	char ts_buf[64] = {0};
	a2_error("[parse error@line: %d]: the token \'%s \'is not allow.", 
		cur_token.line, 
		a2_token2str(&(cur_token), ts_buf));
	}	
	assert(0);
}

// parse global segment
static void parse_gsegment(struct a2_parse* parse_p){
	for( ; !is_end; ){
		parse_segcontent(parse_p);
		// TODO: IR parser
	}
}

// parse local segment
static size_t parse_lsegment(struct a2_parse* parse_p){
	assert(tt2op(parse_readtoken(parse_p)->tt)=='{');	// jump {
	size_t front = 0, back = 0;
	for(; !is_end; ){
		if(cur_token.tt == kp2tt(tk_op, '}')){	// the segment end
			parse_readtoken(parse_p);
			return front;
		}
		if(front == 0){
			front = back = parse_segcontent(parse_p);
		}else{
			node_p(back)->next = parse_segcontent(parse_p);
			back = node_p(back)->next;
		}
	}

	parse_error("the segment lost '}'.");
	assert(0);
	return 0;
}

static inline size_t parse_expression(struct a2_parse* parse_p){
	return _parse_expression(parse_p, parse_operation);
}

static inline size_t parse_exp(struct a2_parse* parse_p){
	return _parse_expression(parse_p, parse_op);
}

static   size_t _parse_expression(struct a2_parse* parse_p, parse_func pfunc){
	switch(tt2tk(cur_token.tt)){
		case tk_ide:{
			struct a2_token* tp = parse_matchtoken(parse_p, 1);
			if(tp == NULL)
				parse_error("expression is invalid.");
			switch(tt2tk(tp->tt)){
				case tk_op:
					// TODO: op
					return pfunc(parse_p);
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
		case tk_key:
			if(a2_ktisfunction(parse_p->env_p, &cur_token)==a2_fail)
				goto EXP_ERROR;
			else
				return parse_function(parse_p);
			break;
		case tk_string:
		case tk_number:
			return pfunc(parse_p);
		case tk_op:{
			switch(tt2op(cur_token.tt)){
				case '[':
					return	parse_array(parse_p);
				case '{':
					return  parse_map(parse_p);
				case '(':
					return pfunc(parse_p);
				default:
					goto EXP_ERROR;
			}
		}
			break;
		default:
			goto EXP_ERROR;
	}

EXP_ERROR:{
		char ts_buf[64] = {0};
		a2_error("[parse error@line: %d]: the token \' %s \' is error grammar.", 
			cur_token.line, a2_token2str(&cur_token, ts_buf));
	}
	return 0;
}

static size_t parse_map(struct a2_parse* parse_p){
	return 0;
}

static size_t parse_array(struct a2_parse* parse_p){
	return 0;
}

// root =  '='   include ,
static inline  size_t parse_operation(struct a2_parse* parse_p){
	return _parse_operation(parse_p, parse_comma);
}

// root = '=' not include ,
static inline  size_t parse_op(struct a2_parse* parse_p){
	return _parse_operation(parse_p, parse_logic);
}

static  size_t _parse_operation(struct a2_parse* parse_p, parse_func pfunc){
	size_t head, exp2;
	size_t exp1 = pfunc(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	switch(tt2op(tp->tt)){
		case '=':
			// check left vaule 
			if( nt2ne(node_p(exp1)->type)==ne_left )
				head = new_node(parse_p, &cur_token, ass_node);
			else
				parse_error("the expression is can not left value");
			break;
		default:
			assert(0);	
	}

	parse_readtoken(parse_p);		// jump = 
	exp2 = pfunc(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;
}

// root = ','
static size_t parse_comma(struct a2_parse* parse_p){
	size_t head=0, back=0;
	back = head = parse_logic(parse_p);
	if(!head) return 0;

	for(; !is_end; ){
		if( tt2op(parse_readtoken(parse_p)->tt)!=',' )
			break;
		node_p(back)->next = parse_logic(parse_p);
		back = node_p(back)->next;
	}  

	return head;
}

// root = '|' 
static size_t parse_logic(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_interval(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	if(tt2op(tp->tt) == '|'){
		head = new_node(parse_p, &cur_token, or_node);
		parse_readtoken(parse_p);
		exp2 = parse_interval(parse_p);
		node_p(head)->childs[0] = exp1;
		node_p(head)->childs[1] = exp2;
		return head;
	}else{
		char ts_buf[32] = {0};
		a2_error("[parse error@line: %d]: the token \'%s\' is grammar error.", 
			cur_token.line, 
			a2_token2str(&cur_token, ts_buf));
	}
	return exp1;
}

// root = '&'
static size_t parse_interval(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_limits(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	if(tt2op(tp->tt) == '&'){
		head = new_node(parse_p, &cur_token, and_node);
		parse_readtoken(parse_p);
		exp2 = parse_limits(parse_p);
		node_p(head)->childs[0] = exp1;
		node_p(head)->childs[1] = exp2;
		return head;
	}else{
		char ts_buf[32] = {0};
		a2_error("[parse error@line: %d]: the token \'%s\' is grammar error.", 
			cur_token.line, 
			a2_token2str(&cur_token, ts_buf));
	}
	return exp1;
}

// root = '>' '<' '>=' '<='
static size_t parse_limits(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_arithmetic(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	switch(tt2op(tp->tt)){
		case '>':
			head = new_node(parse_p, &cur_token, gt_node);
			break;
		case '<':
			head = new_node(parse_p, &cur_token, lt_node);
			break;
		case (('<'<<8)+'='):
			head = new_node(parse_p, &cur_token, gte_node);
			break;
		case (('>'<<8)+'='):
			head = new_node(parse_p, &cur_token, lte_node);
			break;
		case (('='<<8)+'='):
			head = new_node(parse_p, &cur_token, equ_node);
			break;
		case (('!'<<8)+'='):
			head = new_node(parse_p, &cur_token, ne_node);
			break;
		default:
			assert(0);
	}

	parse_readtoken(parse_p);
	exp2 = parse_arithmetic(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;
}

// root= '+' '-'
static size_t parse_arithmetic(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_advanced(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	switch(tt2op(tp->tt)){
		case '+':
			head = new_node(parse_p, &cur_token, add_node);
			break;
		case '-':
			head = new_node(parse_p, &cur_token, sub_node);
			break;
		default:
			assert(0);
	}

	parse_readtoken(parse_p);
	exp2 = parse_advanced(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;
}

// root = '*' '/'
static size_t parse_advanced(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_base(parse_p);
	struct a2_token* tp = parse_attoken(parse_p);

	if( !tp || tt2tk(tp->tt)==tk_end) return exp1;

	switch(tt2op(tp->tt)){
		case '*':
			head = new_node(parse_p, &cur_token, mul_node);
			break;
		case '/':
			head = new_node(parse_p, &cur_token, div_node);
			break;
		default:
			assert(0);
	}

	parse_readtoken(parse_p);
	exp2 = parse_base(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;

}

// root = '!' '-' '+' '..' '.' ('
static size_t parse_base(struct a2_parse* parse_p){
	size_t head;
	struct a2_token* tp = parse_attoken(parse_p);
	
	if(!tp) return 0;
	
	switch(tt2tk(tp->tt)){
		case tk_ide:{
			head = new_node(parse_p, &cur_token, var_node);
			parse_readtoken(parse_p);
			struct a2_token* ntp = parse_attoken(parse_p);
			if(!ntp) return head;
			if(tt2tk(ntp->tt)!=tk_op){
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: the token \'%s\' is not operation token.", 
					cur_token.line, a2_token2str(&cur_token,ts_buf));
			}else{
				switch(tt2op(ntp->tt)){
					case '(':{		// function call
						head = new_node(parse_p, tp, cfunc_node);
						node_p(head)->childs[0] = parse_cargs(parse_p);
					}
						return head;
					case '[':{		// map or array 
						head = new_node(parse_p, tp, cma_node);
						parse_readtoken(parse_p); // jump [
						node_p(head)->childs[0] = parse_expression(parse_p);
						if(tt2op(parse_readtoken(parse_p)->tt)!=']')
							parse_error("you lost \']\'.");
					}
						return head;
					case '.':{
						head = new_node(parse_p, tp, chi_node);
						node_p(head)->next = parse_child(parse_p);
					}
						return head;
					case (('.'<<8)+'.'):{	// string cats
						head = new_node(parse_p, tp, strcat_node);
						node_p(head)->next = parse_stringcats(parse_p);
					}
						return head;
					default:
						return head;
				}
			}
		}
			break;
		case tk_op:{
			switch(tt2op(tp->tt)){
				case '!':{
					head = new_node(parse_p, &cur_token, not_node);
					parse_readtoken(parse_p);
					node_p(head)->childs[0] = parse_expression(parse_p);
					return head;
				}
					break;
				case '-':{
					head = new_node(parse_p, &cur_token, neg_node);
					parse_readtoken(parse_p);
					node_p(head)->childs[0] = parse_expression(parse_p);
					return head;
				}
					break;
				case '(':{
					parse_readtoken(parse_p);
					head = parse_expression(parse_p);
					if(tt2op(parse_readtoken(parse_p)->tt)!=')')
						parse_error("you lost \' ) \'.");
					else
						return head;
				}
					break;
			}
		}
			break;
		default:
			assert(0);
	}
	return head;
}

static size_t parse_child(struct a2_parse* parse_p){
	size_t head=0, back=0;
	struct a2_token* tp=NULL;
	for(; !is_end; ){
		if(tt2op(parse_attoken(parse_p)->tt)=='.')
			parse_readtoken(parse_p); // jump .
		else
			break;
		tp = parse_attoken(parse_p);
		if(!tp)
			parse_error("the \' . \' operation lost variable.");
		if(tt2tk(tp->tt)!=tk_ide){
			char ts_buf[64] = {0};
			a2_error("[parse error@line: %d]: the token \' %s \', is not ungrammatical.",
				&cur_token, a2_token2str(&cur_token, ts_buf));
		}
		if(!head)
			head = back = new_node(parse_p, &cur_token, chi_node);
		else{
			node_p(back)->next = new_node(parse_p, &cur_token, chi_node);
			back = node_p(back)->next;
		}

		parse_readtoken(parse_p);
	}

	return head;
}


static  size_t parse_function(struct a2_parse* parse_p){
	size_t head;
	struct a2_token* tp=NULL;
	parse_readtoken(parse_p);	// jump  function
	if(tt2tk(parse_attoken(parse_p)->tt)==tk_ide)
		tp = parse_readtoken(parse_p);

	head = new_node(parse_p, tp, func_node);
	if( tt2op(parse_attoken(parse_p)->tt)!='(' )
		parse_error("the function's grammar is error, you lost \'(\' after \'function\'.");
	node_p(head)->childs[0] = parse_args(parse_p);
	node_p(head)->childs[1] = parse_lsegment(parse_p);
	return head;
}

// parse call args
static size_t parse_cargs(struct a2_parse* parse_p){
	size_t head=0, back=0;
	struct a2_token* tp;
	parse_readtoken(parse_p);
	for(; !is_end; ){
		tp = parse_attoken(parse_p);
		if( tt2tk(tp->tt)==tk_args ){
			parse_readtoken(parse_p); // JUMP ...
			if( tt2op(parse_readtoken(parse_p)->tt)==')' ){
				if(!head)	
					head = new_node(parse_p, NULL, args_node);
				else{
					node_p(back)->next = new_node(parse_p, NULL, args_node);
				} 
				break;
			}else
				parse_error("the variable parameters list must be last arg.");
		}else{
			if(tt2op(parse_attoken(parse_p)->tt)==')'){
				parse_readtoken(parse_p);
				break;
			}
			if(!head)
				head = back = parse_exp(parse_p);
			else{
				node_p(back)->next = parse_exp(parse_p);
				back = node_p(back)->next;
				if(tt2op(parse_readtoken(parse_p)->tt)!=',')
					parse_error("the parameters is lost \' , \'.");
			}

		}
	}

	return head;
}

static size_t parse_args(struct a2_parse* parse_p){
	size_t head=0, back=0;
	parse_readtoken(parse_p);  // jump (
	struct a2_token* tp;
	for( ;!is_end; ){
		tp = parse_attoken(parse_p);
		switch(tt2tk(tp->tt)){
			case tk_ide:
					if(!head)
						head = back = new_node(parse_p, &cur_token, var_node);
					else{
						node_p(back)->next = new_node(parse_p, &cur_token, var_node);
						back = node_p(back)->next;
					}
					parse_readtoken(parse_p);
					if(tt2op(parse_readtoken(parse_p)->tt)!=',')
						parse_error("you are lost \' , \'.");
				break;
			case tk_args:{
				parse_readtoken(parse_p);	// jump ...
				if(tt2op(parse_readtoken(parse_p)->tt)==')')
					return head;
				else
					parse_error("you are lost \' ) \'.");
			}
				break;
			case tk_op:
				if(tt2op(tp->tt)==')'){
					parse_readtoken(parse_p);
					return head;
				}
			default:{
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: the token \' %s \' is ungrammatical. ", 
					cur_token.tt, a2_token2str(&cur_token, ts_buf));
			}
		}
	}

	parse_error("you are lost \' ) \'.");
	return 0;

}

static size_t parse_stringcats(struct a2_parse* parse_p){
	return 0;
}

static  size_t parse_for(struct a2_parse* parse_p){
	return 0;
}

static  size_t parse_foreach(struct a2_parse* parse_p){
	return 0;
}

static  size_t parse_if(struct a2_parse* parse_p){
	return 0;
}

