#ifndef _A2_IR_H_
#define _A2_IR_H_
#include "a2_conf.h"
#include "a2_env.h"

typedef uint32_t  ir;

// ir code is included 4 byte, the 4byte like that:
//  thanks for lua
#define OP_SIZE 6
#define A_SIZE  8
#define B_SIZE  9
#define C_SIZE  9
#define BX_SIZE (B_SIZE+C_SIZE)

#define OP_POS (A_SIZE+BX_SIZE)
#define A_POS (BX_SIZE)
#define B_POS (C_SIZE)

#define A_MAX  ((1<<A_SIZE)-1)
#define B_MAX  ((1<<B_SIZE)-1)
#define C_MAX  B_MAX
#define BX_MAX ((1<<BX_SIZE)-1)

#define is_limit(i,m) ((i)<0 && (-1-(i))>(m))
#define is_Alimit(i) is_limit(i, A_MAX)
#define is_Blimit(i) is_limit(i, B_MAX)
#define is_Climit(i) is_limit(i, C_MAX)
#define is_BXlimit(i) is_limit(i, BX_MAX)

#define ABC_MODE 0
#define ABX_MODE 1	

#define ir_abc(op,a,b,c)  (((op)<<OP_POS)|((a)<<A_POS)|((b)<<B_POS)|(c))
#define ir_abx(op,a,bx)   ((((1<<(OP_SIZE-1))|op)<<OP_POS)|(a)<<A_POS|(bx))  
#define ir_mode(ir) 	  ((ir)>>(sizeof(ir)-1))

enum ir_op{
	NIL,  // nil op
	GETGLOBAL,  // get global variable
	SETGLOBAL,  // set global variable 
	GETUPVALUE,	// get upvalue
	SETUPVALUE,	// set upvalue
	LOAD,	 	// load const value to register
	ADD,	 	// +
	SUB,	 	// -
	MUL,		// *
	DIV 		// /
};


struct a2_ir;

struct a2_ir* a2_ir_open(struct a2_env* env);
void a2_ir_close(struct a2_ir* ir_p);
void a2_ir_clear(struct a2_ir* ir_p);
inline void a2_ir_exec(struct a2_ir* ir_p, size_t root);

#endif
