#ifndef _A2_PARSE_H_
#define _A2_PARSE_H_
#include "a2_lex.h"


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
	local_node, // local 
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
inline struct a2_node* a2_node_p(struct a2_parse* parse_p, size_t idx);

struct a2_parse;

struct a2_parse*  a2_parse_open(struct a2_env* env_p);
void a2_parse_close(struct a2_parse* parse_p);
void a2_parse_run(struct a2_parse* parse_p, struct a2_token* token_chain, size_t len);
void a2_parse_clear(struct a2_parse* parse_p);


// for test
size_t parse_run(struct a2_parse* parse_p, struct a2_token* token_chain, size_t len);
void dump_node(struct a2_parse* parse_p, size_t root);

#endif
