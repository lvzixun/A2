#ifndef _A2_IR_H_
#define _A2_IR_H_
#include "a2_conf.h"
#include "a2_env.h"

typedef uint32  ir;
typedef int32   sir;
struct a2_xclosure;

//  thanks for lua
#define IR_SIZE  32
#define OP_SIZE 6
#define A_SIZE  8
#define B_SIZE  9
#define C_SIZE  9
#define BX_SIZE (B_SIZE+C_SIZE)

#define OP_POS (A_SIZE+BX_SIZE)
#define A_POS (BX_SIZE)
#define B_POS (C_SIZE)
#define C_POS (0)
#define BX_POS (C_POS)

#define ARG_MAX (A_MAX)
#define A_MAX  ((1<<A_SIZE)-1)
#define B_MAX  ((1<<(B_SIZE-1))-1)
#define C_MAX  B_MAX
#define BX_MAX ((1<<(BX_SIZE-1))-1)

#define is_limit(i,m) ((i)<0 && (-1-(i))>(m))
#define is_Alimit(i)  (i<0)
#define is_Blimit(i) is_limit(i, B_MAX)
#define is_Climit(i) is_limit(i, C_MAX)
#define is_BXlimit(i) is_limit(i, BX_MAX)
#define is_SXlimit(i) is_limit(i, SX_MAX)

#define MODE_BYTESLEN ((((1<<OP_SIZE)+8)&(~0x07))/8)
#define ABC_MODE 0
#define ABX_MODE 1	

#define ir_maskx(c)	 	  ((((ir)(-1))<<(IR_SIZE-(c)))>>(IR_SIZE-(c)))
#define ir_gmaskx(c, p) 	  (ir_maskx(c)<<(p))
#define ir_mask(x, c, p)  ((ir_maskx(c)&(x))<<(p))

#define ir_abc(op,a,b,c)  (assert(op_modle(op)==ABC_MODE), ( \
									  ir_mask(op,OP_SIZE,OP_POS) |  \
									  ir_mask(a,A_SIZE,A_POS) | \
									  ir_mask(b,B_SIZE,B_POS) | \
									  ir_mask(c,C_SIZE,C_POS) ) )
#define ir_abx(op,a,bx)   (assert(op_modle(op)==ABX_MODE), ( \
									  ir_mask(op, OP_SIZE,OP_POS) | \
									  ir_mask(a,A_SIZE,A_POS) | \
									  ir_mask(bx, BX_SIZE,BX_POS) ) )

#define ir_gop(i)		  ((ir_gmaskx(OP_SIZE, OP_POS) & (i))>>OP_POS)
#define ir_ga(i)		  ((ir_gmaskx(A_SIZE, A_POS) & (i))>>A_POS)
 
#define _ir_sig(v, c) 	  ( ((sir)((v)<<(IR_SIZE-(c))))>>(IR_SIZE-(c)) )

#define _ir_gb(i)		  ((ir_gmaskx(B_SIZE, B_POS) & (i))>>B_POS)
#define ir_gb(i)		  _ir_sig(_ir_gb(i), B_SIZE)


#define _ir_gc(i)		  ((ir_gmaskx(C_SIZE, C_POS) & (i))>>C_POS)
#define ir_gc(i)		  _ir_sig(_ir_gc(i), C_SIZE)


#define _ir_gbx(i)		  ((ir_gmaskx(BX_SIZE, BX_POS) & (i))>>BX_POS)
#define ir_gbx(i)		  _ir_sig(_ir_gbx(i), BX_SIZE)

enum ir_op{
	NIL, 		// nil
	GETGLOBAL,  // get global variable,   modle is abx
	SETGLOBAL,  // set global variable,  modle is abc
	GETUPVALUE,	// get upvalue, modle is abx
	SETUPVALUE,	// set upvalue, modle is abc
	NEWLIST,	// new list, modle is abx
	NEWMAP,		// new map modle is abx
	SETLIST,	// set list,  modle is abc
	SETMAP,		// set map, modle is abc
	GETVALUE,	// get value, modle is abc
	SETVALUE,	// set value, modle is abc
	CLOSURE,	// closure, modle is abx
	CALL,		// call, modle is abc
	RETURN,		// return, modle is abx
	FORPREP,	// for prepare modle is abx
	FORLOOP,	// for loop modle is abx
	FOREACHPREP,// foreach  prep modle is abx 
	FOREACHLOOP,// foreach loop modle is abx
	JUMP,		// jump, modle is sx
	MOVE,		// move, modle is abx
	TEST,		// test, modle is abx
	LOAD,	 	// load const value to register, modle is abx
	LOADNIL,	// reset regs is nil, modle is abx
	CAT,		// .., 
	ADD,	 	// +, modle is abc
	SUB,	 	// -, modle is abc
	MUL,		// *, modle is abc
	DIV, 		// /, modle is abc
	OR,			// |, modle is abc
	AND,		// &, modle is abc
	BIG,		// >, modle is abc
	LITE,		// <, modle is abc
	EQU,		// == , modle is abc
	NEQU,		// !=, modle is abc
	BIGE,		// >=, modle is abc
	LITEE,		// <=, modle is abc
	NOT,		// !
	NEG,		//-a  modle is abx

	ir_count	// op count
};


struct a2_ir;

struct a2_ir* a2_ir_open(struct a2_env* env);
void a2_ir_close(struct a2_ir* ir_p);
inline void a2_ir_newxcls(struct a2_ir* ir_p);
void a2_ir_clear(struct a2_ir* ir_p);
inline void a2_ir_exec(struct a2_ir* ir_p, size_t root);
inline struct a2_xclosure* a2_ir_exend(struct a2_ir* ir_p);

// for test
char* ir2string(struct a2_ir* ir_p, struct a2_xclosure* xcls, ir _ir, char* str, size_t size);
void dump_ir(struct a2_ir* ir_p);

#endif
