#include "a2_conf.h"
#include "a2_lex.h"
#include "a2_env.h"
#include "a2_error.h"
#include <string.h>
#include <stdio.h>

enum {
	ne_right = 0,
	ne_left = 1
};

enum {
	nr_nil = 0,
	nr_read =1
};

#define nt_lmask  ((ne_left)<<8)
#define nt2ne(nt)	(((nt)>>8)&(0x01))

#define nt_rmask  ((nr_read)<<9)
#define nt2nr(nt) (((nt)>>9)*(0x01))


enum node_type{

	// string node
	strcat_node,// ..

	str_node,
	num_node,

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

	bool_node,
	args_node,	// ...
	func_node, 	// def function
	map_node,   // def map
	array_node, // def array
	return_node,
	break_node,
	continue_node,
	for_node,
	foreach_node,

	cfunc_node = (nt_rmask | 0x00),	//  call  func()

	var_node = (nt_rmask | nt_lmask | 0x00),  // variable node
	ass_node = (nt_lmask | 0x01),   // =
	cma_node = (nt_rmask | nt_lmask | 0x02),	// map['key'] or  array[idx]
	chi_node = (nt_rmask | nt_lmask | 0x03), 	// .
	comma_node = (nt_lmask | 0x04) // ,
};

struct a2_node{
	uint16_t type;
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
#define parse_error(s)	a2_error("[parse error@line: %d]: %s\n", (is_end)?(parse_p->token_chain[parse_p->t_idx-1].line):(cur_token.line), (s))

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
static size_t parse_elif(struct a2_parse* parse_p);
static size_t parse_deep(struct a2_parse* parse_p);


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

	return ++(parse_p->cap);
}

static inline void clear_node(struct a2_parse* parse_p){
	parse_p->cap = 0;
}

static inline size_t new_node(struct a2_parse* parse_p, struct a2_token* token,  enum node_type nt){
	size_t idx = get_node(parse_p);
	node_p(idx)->next=0;
	node_p(idx)->token = token;
	node_p(idx)->type = nt;
	memset(node_p(idx)->childs, 0, sizeof(node_p(idx)->childs));
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

static size_t parse_return(struct a2_parse* parse_p){
	size_t head;
	struct a2_token* tp = parse_readtoken(parse_p); // jump return
	head = new_node(parse_p, tp, return_node);
	node_p(head)->childs[0] = parse_expression(parse_p);
	return head;
}

static size_t parse_segcontent(struct a2_parse* parse_p){
	switch( tt2tk(cur_token.tt) ){
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
				return parse_return(parse_p);
			}else if(a2_ktisif(parse_p->env_p, &cur_token) == a2_true){	// parse if
				return 	parse_if(parse_p);
			}else if(a2_ktisbreak(parse_p->env_p, &cur_token) == a2_true){  // parse break
				return new_node(parse_p, &cur_token, break_node);
			}else if(a2_ktiscontinue(parse_p->env_p, &cur_token)== a2_true){  // parse continue
				return new_node(parse_p, &cur_token, continue_node);
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
				case '[':
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
	a2_error("[parse error@line: %d]: the token \'%s \'is do not expect.\n", 
		cur_token.line, 
		a2_token2str(&(cur_token), ts_buf));
	}	
	assert(0);
}

// parse global segment
static void parse_gsegment(struct a2_parse* parse_p){
	for( ; !is_end; ){
		parse_segcontent(parse_p);
		if(!is_end && tt2tk(cur_token.tt)==tk_end)
			parse_readtoken(parse_p);
		// TODO: IR parser

	}
}

// parse local segment
static size_t parse_lsegment(struct a2_parse* parse_p){
	struct a2_token* tp = parse_attoken(parse_p);
	if(!tp || tt2op(parse_readtoken(parse_p)->tt)!='{')  	// jump {
		parse_error("you lost \' { \'.");
		
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

		if(!is_end && tt2tk(cur_token.tt)==tk_end)
			parse_readtoken(parse_p);
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
			if(tp == NULL) return new_node(parse_p, parse_readtoken(parse_p), var_node);
			switch(tt2tk(tp->tt)){
				case tk_op:
					// TODO: op
					return pfunc(parse_p);
				case tk_end:{
					size_t head = new_node(parse_p, parse_readtoken(parse_p), var_node);
					parse_readtoken(parse_p);
					return head;
				}
				default:{
					char ts_buf[64] = {0};
					a2_error("[parse error@line: %d]: at token \'%s\' is error grammar.\n",
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
		case tk_bool:
		case tk_string:
		case tk_number:
			return pfunc(parse_p);
		case tk_op:{
			switch(tt2op(cur_token.tt)){
				case '(':
				case '{':
				case '[':
				case '+':
				case '-':
					return pfunc(parse_p);
				default:
					goto EXP_ERROR;
			}
		}
			break;
		default:
			assert(0);
			goto EXP_ERROR;
	}

EXP_ERROR:{
		char ts_buf[64] = {0};
		a2_error("[parse error@line: %d]: the token \' %s \' is error grammar.\n", 
			cur_token.line, a2_token2str(&cur_token, ts_buf));
	}
	return 0;
}

static size_t parse_map(struct a2_parse* parse_p){
	size_t head=0, back=0, nn=0;
	struct a2_token* tp = NULL;
	tp = parse_readtoken(parse_p);
	assert(tp && tt2op(tp->tt)=='{'); // JUMP  {		
	back = head = new_node(parse_p, tp, map_node);
	tp = parse_attoken(parse_p);
	if(!tp && tt2op(tp->tt)=='}'){
		parse_readtoken(parse_p);
		return head;	
	} 
	for(;!is_end; ){
		if(tt2tk(cur_token.tt)==tk_end){   // jump end
			parse_readtoken(parse_p);
			continue;
		}

		switch(tt2tk(cur_token.tt)){
			case tk_ide:
				nn = new_node(parse_p, &cur_token, var_node);
				break;
			case tk_number:
				nn = new_node(parse_p, &cur_token, num_node);
				break;
			case tk_string:
				nn = new_node(parse_p, &cur_token, str_node);
				break;
			default:{
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: the token \' %s \' is can not set key.\n",
					cur_token.line, a2_token2str(&cur_token, ts_buf));
			}
		}

		parse_readtoken(parse_p);		// jump  key
		if(is_end || tt2op(parse_readtoken(parse_p)->tt)!='=') // jump '='
			parse_error("you lost \' = \'.");
		node_p(nn)->childs[0] = parse_exp(parse_p);
		node_p(back)->next = nn;
		back = node_p(back)->next;

MAP_PASS:		
		tp = parse_readtoken(parse_p);
		if(!tp) goto MAP_END;
		if(tt2tk(tp->tt)==tk_end) goto MAP_PASS;
		switch(tt2op(tp->tt)){
			case ',':
				break;
			case '}':
				return head;
			default:{
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: you lost \' , \' or \' } \' before the token \' %s \'.\n",
					tp->line, a2_token2str(tp, ts_buf));
			}
		}
	}

MAP_END:
	parse_error("you lost \' } \'.");
	return 0;
}

static size_t parse_array(struct a2_parse* parse_p){
	size_t head=0, back=0;
	struct a2_token* tp = NULL;
	tp = parse_readtoken(parse_p);
	assert(tp && tt2op(tp->tt)=='['); // jump '['
	back = head = new_node(parse_p, tp, array_node);
	tp = parse_attoken(parse_p);
	if(!tp && tt2op(tp->tt)==']') {
		parse_readtoken(parse_p);
		return head;
	}

	for(; !is_end;){
		if(tt2tk(cur_token.tt)==tk_end){   // jump end
			parse_readtoken(parse_p);
			continue;
		}

		node_p(back)->next = parse_exp(parse_p);
		back = node_p(back)->next;
		tp = parse_readtoken(parse_p);
		if(!tp)  goto ARRAY_END;
		if(tt2op(tp->tt)==',') continue;  // jump ','
		else if(tt2op(tp->tt)==']') return head;
		else parse_error("you lost \' , \'.");
	}
ARRAY_END:
	parse_error("you lost \' ] \'.");
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
		case '=':{
			// check left vaule
			size_t texp1 = exp1;
			for(;texp1;texp1=node_p(texp1)->next){
				if( nt2ne(node_p(texp1)->type)!=ne_left )
					parse_error("the expression is can not left value");
			}
			head = new_node(parse_p, &cur_token, ass_node);
		}
			break; 
		default:
			return exp1;	
	}

	parse_readtoken(parse_p);		// jump = 
	exp2 = parse_expression(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;
}

// root = ','
static size_t parse_comma(struct a2_parse* parse_p){
	size_t head=0, back=0;
	back = head = parse_logic(parse_p);
	if(!head) return 0;

	if(!is_end && tt2op(cur_token.tt)==','){
		head = new_node(parse_p, &cur_token, comma_node);
		node_p(head)->childs[0] = back;
	}
	for(; !is_end; ){
		if( tt2op(cur_token.tt)!=',' )
			break;
		parse_readtoken(parse_p);
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
		exp2 = parse_logic(parse_p);
		node_p(head)->childs[0] = exp1;
		node_p(head)->childs[1] = exp2;
		return head;
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
		exp2 = parse_interval(parse_p);
		node_p(head)->childs[0] = exp1;
		node_p(head)->childs[1] = exp2;
		return head;
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
			return exp1;
	}

	parse_readtoken(parse_p);
	exp2 = parse_limits(parse_p);
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
			return exp1;
	}

	parse_readtoken(parse_p);
	exp2 = parse_arithmetic(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;
}

// root = '*' '/'
static size_t parse_advanced(struct a2_parse* parse_p){
	size_t head, exp2;
	size_t exp1 = parse_deep(parse_p);
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
			return exp1;
	}

	parse_readtoken(parse_p);
	exp2 = parse_advanced(parse_p);
	node_p(head)->childs[0] = exp1;
	node_p(head)->childs[1] = exp2;
	return head;

}


// root = .. function call map and array access
static size_t parse_deep(struct a2_parse* parse_p){
	size_t head;
	size_t exp1 = parse_base(parse_p);
	struct a2_token* tp = NULL;
	int flag = 0;
	do{
		tp = parse_attoken(parse_p);
		if(!tp || tt2tk(tp->tt)==tk_end) return exp1;
		switch(tt2op(tp->tt)){
			case (('.'<<8)+'.'):{
				if(!exp1) parse_error("you lost ide before \'..\'.");
				head = new_node(parse_p, parse_readtoken(parse_p), strcat_node);
				node_p(head)->childs[0] = exp1;
				node_p(head)->childs[1] = parse_deep(parse_p);
				return head;
			}
				break;
			case '[':
				if(!exp1) parse_error("you lost ide before \'[\'.");
				if(nt2nr(node_p(exp1)->type)){
					head = new_node(parse_p, &cur_token, cma_node);
					parse_readtoken(parse_p); // jump [
					node_p(head)->childs[0] = exp1;
					node_p(head)->childs[1] = parse_exp(parse_p);
					if(tt2op(parse_readtoken(parse_p)->tt)!=']')
						parse_error("you lost \' ] \'.");
					exp1 = head;
					flag = 1;
					continue;
				}

				print_token(node_p(exp1)->token);
				print_token(&cur_token);
				printf("exp1.type = %d\n", node_p(exp1)->type);
				parse_error("the [] left operation can not be read.");
				break;
			case '(':
				assert(exp1);
				if(nt2nr(node_p(exp1)->type)){
					head = new_node(parse_p, &cur_token, cfunc_node);
					node_p(head)->childs[0] = exp1;
					node_p(head)->childs[1] = parse_cargs(parse_p);
					exp1 = head;
					flag = 1;
					continue;
				}
				parse_error("the expression can not be call.");
				break;
			case '.':
				if(!exp1) parse_error("the \' . \' operation's left value is not exist.");
				if(nt2nr(node_p(exp1)->type) || node_p(exp1)->type==map_node){
					head = new_node(parse_p, &cur_token, chi_node);
					parse_readtoken(parse_p);  // jump .
					node_p(head)->childs[0] = exp1;
					if(tt2tk(cur_token.tt)!=tk_ide)
						parse_error("the \' . \' operation right must is ide.");
					node_p(head)->childs[1] = new_node(parse_p, parse_readtoken(parse_p), var_node);
					exp1 = head;
					flag = 1;
					continue;
				}
				parse_error("the \' . \' operation's left value is not allow.");
				break;
			default:
				return exp1;
		}
	}while(flag);

	assert(0);
	return 0;
}


// root = '!' '-' '+' '..' '.' ('
static size_t parse_base(struct a2_parse* parse_p){
	size_t head;
	struct a2_token* tp = parse_attoken(parse_p);
	if(!tp) parse_error("the base expression is pointless.");
	
	switch(tt2tk(tp->tt)){
		case tk_ide:{
			struct a2_token* ntp = parse_matchtoken(parse_p, 1);
			if(!ntp || tt2tk(ntp->tt)==tk_end) goto BASE_DEF;
			if(tt2tk(ntp->tt)!=tk_op){
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: the token \'%s\' is not operation token.", 
					cur_token.line, a2_token2str(&cur_token,ts_buf));
			}else{
BASE_DEF:
				return new_node(parse_p, parse_readtoken(parse_p), var_node);
			}
		}
			break;
		case tk_bool:
			return new_node(parse_p, parse_readtoken(parse_p), bool_node);
		case tk_string:
			return new_node(parse_p, parse_readtoken(parse_p), str_node);
		case tk_number:
			return new_node(parse_p, parse_readtoken(parse_p), num_node);
		case tk_op:{
			switch(tt2op(tp->tt)){
				case '[':
					return	parse_array(parse_p);
				case '{':
					return  parse_map(parse_p);
				case '!':
					head = new_node(parse_p, parse_readtoken(parse_p), not_node);
					node_p(head)->childs[0] = parse_base(parse_p);
					return head;
				case '-':
					head = new_node(parse_p, parse_readtoken(parse_p), neg_node);
					node_p(head)->childs[0] = parse_base(parse_p);
					return head;
				case '+':
					parse_readtoken(parse_p);
					return parse_base(parse_p);
				case '(':
					parse_readtoken(parse_p);
					head = parse_expression(parse_p);
					if(tt2op(parse_readtoken(parse_p)->tt)!=')')
						parse_error("you lost \' ) \'.");
					else
						return head;
			}
		}
			break;
		default:{
			char ts_buf[64] = {0};
			a2_error("[parse error@line: %d]: the token \' %s \' is do not expect`.\n",
				cur_token.line, a2_token2str(&cur_token, ts_buf));
		}
	}
	return 0;
}

static  size_t parse_function(struct a2_parse* parse_p){
	size_t head;
	struct a2_token* tp=NULL;
	parse_readtoken(parse_p);	// jump  function

	tp = parse_attoken(parse_p);
	if(!tp) parse_error("you function is ungrammatical.");
	if(tt2tk(tp->tt)==tk_ide)
		tp = parse_readtoken(parse_p);
	else
		tp = NULL;

	head = new_node(parse_p, tp, func_node);
	tp= parse_attoken(parse_p);
	if( !tp || tt2op(tp->tt)!='(' )
		parse_error("the function's grammar is error, you lost \'(\' after \'function\'.");
	node_p(head)->childs[0] = parse_args(parse_p);
	node_p(head)->childs[1] = parse_lsegment(parse_p);
	return head;
}

// parse call args
static size_t parse_cargs(struct a2_parse* parse_p){
	size_t head=0, back=0;
	struct a2_token* tp;
	tp = parse_readtoken(parse_p);  // jump '('
	assert(!tp && tt2op(tp->tt)=='(');
	tp = parse_attoken(parse_p);
	if(!tp && tt2op(tp->tt)==')'){
		parse_readtoken(parse_p);
		return 0;
	}

	for(; !is_end; ){
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
			if(!head)
				head = back = parse_exp(parse_p);
			else{
				node_p(back)->next = parse_exp(parse_p);
				back = node_p(back)->next;
			}
			tp = parse_readtoken(parse_p);
			if(!tp) goto CARGS_END;
			if(tt2op(tp->tt)==')') return head;
			else if(tt2op(tp->tt)==',') continue;
			else parse_error("the parameters is lost \' , \'.");
		}
	}

CARGS_END:
	parse_error("you lost \' ) \'.");
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
					head = back = new_node(parse_p, parse_readtoken(parse_p), var_node);
				else{
					node_p(back)->next = new_node(parse_p, parse_readtoken(parse_p), var_node);
					back = node_p(back)->next;
				}
				break;
			case tk_args:{
				parse_readtoken(parse_p);	// jump ...
				if(tt2op(parse_readtoken(parse_p)->tt)==')')
					return head;
				else
					parse_error("the last args must is \' ... \'.");
			}
				break;
			default:{
				char ts_buf[64] = {0};
				a2_error("[parse error@line: %d]: the token \' %s \' is ungrammatical. ", 
					cur_token.tt, a2_token2str(&cur_token, ts_buf));
			}
		}

		tp = parse_readtoken(parse_p);
		if(!tp) break;
		if(tt2op(tp->tt)==',') continue;
		else if(tt2op(tp->tt)==')') return head;
	}

	parse_error("you are lost \' ) \'.");
	return 0;
}

static inline int match_op(struct a2_parse* parse_p, char c){
	struct a2_token* tp = parse_readtoken(parse_p);
	return (!tp || tt2op(tp->tt)!= c)?(a2_fail):(a2_true);
}

static  size_t parse_for(struct a2_parse* parse_p){
	size_t head=0;
	assert(a2_ktisfor(parse_p->env_p, &cur_token)==a2_true);
	struct a2_token* tp = parse_readtoken(parse_p);	//JUMP for

	if(match_op(parse_p, '(')==a2_fail) parse_error("you lost \' ( \' after for.");

	head = new_node(parse_p, tp, for_node);
	node_p(head)->childs[0] = parse_exp(parse_p);
	node_p(head)->childs[1] = parse_logic(parse_p);
	node_p(head)->childs[2] = parse_exp(parse_p);

	if(match_op(parse_p, ')')==a2_fail) parse_error("you lost \' ) \' after for.");

	tp = parse_attoken(parse_p);
	if(!tp) parse_error("you lost \' { \' after for expression.");
    if(tt2op(tp->tt)!='{')
    	node_p(head)->childs[3] = parse_expression(parse_p);
    else
    	node_p(head)->childs[3] = parse_lsegment(parse_p);

	return head;
}

static  size_t parse_foreach(struct a2_parse* parse_p){
	size_t head=0;
	assert(a2_ktisforeach(parse_p->env_p, &cur_token)==a2_true);
	struct a2_token* tp = parse_readtoken(parse_p); // jump foreach

	if(match_op(parse_p, '(')) 	parse_error("you lost \' ( \' after for.");

	head = new_node(parse_p, tp, foreach_node);
	size_t kvt = node_p(head)->childs[0] = parse_expression(parse_p);
	for( ;kvt; kvt=node_p(kvt)->next){
		if( nt2ne(node_p(kvt)->type)!=ne_left )
			parse_error("the expression is can not left value");
	}

	tp = parse_readtoken(parse_p);
	if(!tp || a2_ktisin(parse_p->env_p, tp)==a2_fail)
		parse_error("you lost \' in \'.");
	node_p(head)->childs[1] = parse_exp(parse_p);

	tp = parse_attoken(parse_p);
	if(!tp) parse_error("you lost \' { \' after for expression.");
    if(tt2op(tp->tt)!='{')
    	node_p(head)->childs[2] = parse_expression(parse_p);
    else
    	node_p(head)->childs[2] = parse_lsegment(parse_p);

	return head;
}

static  size_t parse_if(struct a2_parse* parse_p){
	size_t head=0;
	assert(a2_ktisif(parse_p->env_p, &cur_token)==a2_true);
	struct a2_token* _tp = parse_readtoken(parse_p);  // jump if

	if(match_op(parse_p, '(')==a2_fail) parse_error("you lost \' ( \' after for.");
	head = new_node(parse_p, _tp, if_node);
	node_p(head)->childs[0] = parse_logic(parse_p);
	if(match_op(parse_p, ')')==a2_fail) parse_error("you lost \' ) \' after for.");
	node_p(head)->childs[1] = parse_lsegment(parse_p);
	node_p(head)->childs[2] = parse_elif(parse_p);
	return head;
}

static size_t parse_elif(struct a2_parse* parse_p){
	size_t head=0;
	if( is_end || a2_ktiselse(parse_p->env_p, &cur_token)==a2_fail )
		return 0;
	parse_readtoken(parse_p); // jump else

	struct a2_token* tp = parse_attoken(parse_p);
	if(!tp) parse_error("the else is ungrammatical.");
	if(a2_ktisif(parse_p->env_p, tp)==a2_true){ // else if
		head = parse_if(parse_p);
	}else{
		if(tt2op(cur_token.tt)=='{')  //else
			head = parse_lsegment(parse_p);
		else
			head = parse_expression(parse_p);
	}
	return head;
}

// for test 
void dump_node(struct a2_parse* parse_p, size_t root){
	for( ;root; ){
		char ts_buf[64] = {0};
		printf("[%lu] nt=%d token=\' %s \'\n", root, node_p(root)->type, 
			a2_token2str(node_p(root)->token, ts_buf));
		printf("    childs[%lu %lu %lu %lu] next[%lu]\n", node_p(root)->childs[0],
			node_p(root)->childs[1],node_p(root)->childs[2],node_p(root)->childs[3], node_p(root)->next);
		int i;
		for(i=0; i<4; i++){
			if(node_p(root)->childs[i]==0)
				break;
			dump_node(parse_p, node_p(root)->childs[i]);
		}

		root = node_p(root)->next;
	}
}

static inline size_t _parse_gsegment(struct a2_parse* parse_p){
	size_t head=0, back=0;
	for( ; !is_end; ){
		size_t root = parse_segcontent(parse_p);
		if(!head)
			head = back = root;
		else{
			node_p(back)->next = root;
			back = node_p(back)->next;
		}
		if(!is_end && tt2tk(cur_token.tt)==tk_end)
			parse_readtoken(parse_p);
	}
	return head;
}

size_t parse_run(struct a2_parse* parse_p, struct a2_token* token_chain, size_t len){
	assert(parse_p);
	assert(token_chain);
	assert(len);

	parse_p->len = len;
	parse_p->token_chain = token_chain;
	parse_p->t_idx = 0;

	return _parse_gsegment(parse_p);
}

