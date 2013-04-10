/* the op token ir dispath, but ignore the pattern.
 because the performance was promoted 5% when merge to op token dispath. 
 and the decrease of code readability after merge op token dispath.
 I think the reason is `inline` performance. so I do not intend to merge op token dispath.

 the head file contains the op dispath address. using computed goto.
*/

#ifndef _A2_VM_DISPATH_H_
#define _A2_VM_DISPATH_H_
#include "a2_ir.h"

#define label_op(op_name)    _IR_OP_LABEL_##op_name 
#define dispath_addr(op_name) &&label_op(op_name)
#define dispath_op(op_name)  label_op(op_name): 

#define dispath_begin _IR_OP_LABEL_BEGIN:
#define dispath_goto(dis_ptr, op) goto *(dis_ptr[op])   
#define dispath_end goto _IR_OP_LABEL_BEGIN;


#define dispath_init(dis_ptr) static const void* dis_ptr[] = { \
	dispath_addr(NIL),\
	dispath_addr(GETGLOBAL),\
	dispath_addr(SETGLOBAL),\
	dispath_addr(GETUPVALUE),\
	dispath_addr(SETUPVALUE),\
	dispath_addr(NEWLIST),\
	dispath_addr(NEWMAP),\
	dispath_addr(SETLIST),\
	dispath_addr(SETMAP),\
	dispath_addr(GETVALUE),\
	dispath_addr(SETVALUE),\
	dispath_addr(CLOSURE),\
	dispath_addr(CALL),\
	dispath_addr(RETURN),\
	dispath_addr(FORPREP),\
	dispath_addr(FORLOOP),\
	dispath_addr(FOREACHPREP),\
	dispath_addr(FOREACHLOOP),\
	dispath_addr(JUMP),\
	dispath_addr(MOVE),\
	dispath_addr(TEST),\
	dispath_addr(LOAD),\
	dispath_addr(LOADNIL),\
	dispath_addr(CAT),\
	dispath_addr(ADD),\
	dispath_addr(SUB),\
	dispath_addr(MUL),\
	dispath_addr(DIV),\
	dispath_addr(OR),\
	dispath_addr(AND),\
	dispath_addr(BIG),\
	dispath_addr(LITE),\
	dispath_addr(EQU),\
	dispath_addr(NEQU),\
	dispath_addr(BIGE),\
	dispath_addr(LITEE),\
	dispath_addr(NOT),\
	dispath_addr(NEG)\
}   

#endif
