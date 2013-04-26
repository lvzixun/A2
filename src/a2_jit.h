/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.3.0, DynASM x64 version 1.3.0
** DO NOT EDIT! The original file is in "./src/a2_jitx64.desc".
*/

#if DASM_VERSION != 10300
#error "Version mismatch between DynASM and included encoding engine"
#endif

# 1 "./src/a2_jitx64.desc"
// the A2 JIT for x64 CPU.

//|.arch x64
//|.actionlist actions
static const unsigned char actions[2710] = {
  85,65,87,65,82,83,65,86,65,85,65,84,65,83,81,82,83,72,137,229,73,191,237,
  237,73,139,135,233,76,139,144,233,73,139,135,233,72,139,152,233,72,105,219,
  239,73,3,159,233,255,91,90,89,65,91,65,92,65,93,65,94,91,65,90,65,95,93,195,
  255,185,237,72,186,237,237,137,139,233,72,137,147,233,73,252,255,194,255,
  73,137,221,73,129,197,239,73,137,222,73,129,198,239,249,77,57,252,238,15,
  132,245,49,201,129,201,239,72,49,210,65,137,142,233,73,137,150,233,73,129,
  198,239,252,233,245,249,73,252,255,194,255,73,190,237,237,255,73,137,222,
  73,129,198,239,255,73,139,191,233,76,137,252,246,72,184,237,237,252,255,208,
  73,137,197,73,131,252,253,0,15,133,245,76,137,252,255,76,137,252,246,73,139,
  135,233,76,137,144,233,72,184,237,237,252,255,208,249,65,139,141,233,73,139,
  149,233,137,139,233,72,137,147,233,73,252,255,194,255,73,189,237,237,255,
  73,137,221,73,129,197,239,255,73,139,191,233,76,137,252,246,76,137,252,234,
  72,184,237,237,252,255,208,65,139,141,233,73,139,149,233,137,139,233,72,137,
  147,233,73,252,255,194,255,184,237,129,252,248,239,15,133,245,77,139,183,
  233,73,129,198,239,65,139,142,233,73,139,150,233,137,139,233,72,137,147,233,
  252,233,245,249,129,252,248,239,15,133,245,72,191,237,237,72,184,237,237,
  255,252,255,208,73,137,198,65,139,142,233,73,139,150,233,137,139,233,72,137,
  147,233,249,73,252,255,194,255,184,237,129,252,248,239,15,133,245,72,191,
  237,237,72,184,237,237,252,255,208,73,137,197,65,139,142,233,73,139,150,233,
  65,137,141,233,73,137,149,233,252,233,245,249,129,252,248,239,15,133,245,
  77,139,175,233,73,129,197,239,255,65,139,142,233,73,139,150,233,65,137,141,
  233,73,137,149,233,249,73,252,255,194,255,73,188,237,237,255,73,137,220,73,
  129,196,239,255,65,139,133,233,129,252,248,239,15,133,245,65,139,132,253,
  36,233,129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,
  233,72,184,237,237,252,255,208,249,77,139,157,233,77,139,171,233,252,242,
  73,15,44,132,253,36,233,72,131,252,248,0,15,140,245,77,137,252,235,73,59,
  131,233,15,141,245,255,77,139,179,233,72,193,224,4,73,1,198,252,233,245,249,
  77,49,252,246,249,73,129,252,254,239,15,132,245,65,139,142,233,73,139,150,
  233,137,139,233,72,137,147,233,252,233,245,249,65,139,133,233,129,252,248,
  239,15,133,245,65,139,132,253,36,233,255,129,252,248,239,15,132,245,65,139,
  132,253,36,233,129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,
  137,144,233,72,184,237,237,252,255,208,249,77,139,157,233,77,139,171,233,
  76,137,252,239,76,137,230,72,184,237,237,252,255,208,73,137,198,73,131,252,
  254,0,15,132,245,65,139,142,233,73,139,150,233,255,137,139,233,72,137,147,
  233,252,233,245,249,76,137,252,255,73,139,135,233,76,137,144,233,72,184,237,
  237,252,255,208,249,76,137,252,255,73,139,135,233,76,137,144,233,72,184,237,
  237,252,255,208,249,73,252,255,194,255,77,139,179,233,72,193,224,4,73,1,198,
  252,233,245,249,77,49,252,246,249,73,129,252,254,239,15,132,245,255,65,139,
  141,233,73,139,149,233,65,137,142,233,73,137,150,233,252,233,245,249,65,139,
  133,233,129,252,248,239,15,133,245,65,139,132,253,36,233,129,252,248,239,
  15,132,245,65,139,132,253,36,233,129,252,248,239,15,132,245,76,137,252,255,
  73,139,135,233,255,76,137,144,233,72,184,237,237,252,255,208,249,77,139,157,
  233,77,139,171,233,76,137,252,239,76,137,230,72,184,237,237,252,255,208,73,
  137,198,73,131,252,254,0,15,132,245,255,65,139,141,233,73,139,149,233,65,
  137,142,233,73,137,150,233,252,233,245,249,76,137,252,255,73,139,135,233,
  76,137,144,233,72,184,237,237,252,255,208,249,76,137,252,255,73,139,135,233,
  76,137,144,233,72,184,237,237,252,255,208,249,73,252,255,194,255,65,139,134,
  233,129,252,248,239,15,133,245,65,139,133,233,129,252,248,239,15,133,245,
  73,139,134,233,102,72,15,110,192,73,139,133,233,102,72,15,110,200,252,242,
  15,88,193,102,72,15,126,192,73,137,222,73,129,198,239,185,237,65,137,142,
  233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,135,233,76,137,144,
  233,72,184,237,237,255,65,139,134,233,129,252,248,239,15,133,245,65,139,133,
  233,129,252,248,239,15,133,245,73,139,134,233,102,72,15,110,192,73,139,133,
  233,102,72,15,110,200,252,242,15,92,193,102,72,15,126,192,73,137,222,73,129,
  198,239,185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,
  255,73,139,135,233,76,137,144,233,72,184,237,237,255,65,139,134,233,129,252,
  248,239,15,133,245,65,139,133,233,129,252,248,239,15,133,245,73,139,134,233,
  102,72,15,110,192,73,139,133,233,102,72,15,110,200,252,242,15,94,193,102,
  72,15,126,192,73,137,222,73,129,198,239,185,237,65,137,142,233,73,137,134,
  233,252,233,245,249,76,137,252,255,73,139,135,233,76,137,144,233,72,184,237,
  237,255,65,139,134,233,129,252,248,239,15,133,245,65,139,133,233,129,252,
  248,239,15,133,245,73,139,134,233,102,72,15,110,192,73,139,133,233,102,72,
  15,110,200,252,242,15,89,193,102,72,15,126,192,73,137,222,73,129,198,239,
  185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,
  135,233,76,137,144,233,72,184,237,237,255,65,139,134,233,129,252,248,239,
  15,133,245,65,139,133,233,129,252,248,239,15,133,245,73,139,134,233,102,72,
  15,110,192,73,139,133,233,102,72,15,110,200,102,15,46,193,15,151,208,36,1,
  72,15,182,192,102,72,15,110,192,102,72,15,126,192,73,137,222,73,129,198,239,
  185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,
  135,233,76,137,144,233,72,184,237,237,255,65,139,134,233,129,252,248,239,
  15,133,245,65,139,133,233,129,252,248,239,15,133,245,73,139,134,233,102,72,
  15,110,192,73,139,133,233,102,72,15,110,200,102,15,46,200,15,151,208,36,1,
  72,15,182,192,102,72,15,110,192,102,72,15,126,192,73,137,222,73,129,198,239,
  185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,
  135,233,76,137,144,233,72,184,237,237,255,65,139,134,233,129,252,248,239,
  15,133,245,65,139,133,233,129,252,248,239,15,133,245,73,139,134,233,102,72,
  15,110,192,73,139,133,233,102,72,15,110,200,102,15,46,193,15,147,208,36,1,
  72,15,182,192,102,72,15,110,192,102,72,15,126,192,73,137,222,73,129,198,239,
  185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,
  135,233,76,137,144,233,72,184,237,237,255,65,139,134,233,129,252,248,239,
  15,133,245,65,139,133,233,129,252,248,239,15,133,245,73,139,134,233,102,72,
  15,110,192,73,139,133,233,102,72,15,110,200,102,15,46,200,15,147,208,36,1,
  72,15,182,192,102,72,15,110,192,102,72,15,126,192,73,137,222,73,129,198,239,
  185,237,65,137,142,233,73,137,134,233,252,233,245,249,76,137,252,255,73,139,
  135,233,76,137,144,233,72,184,237,237,255,73,137,222,73,129,198,239,65,139,
  142,233,73,139,150,233,137,139,233,72,137,147,233,73,252,255,194,255,73,137,
  222,73,129,198,239,65,139,134,233,129,252,248,239,15,132,245,65,139,134,233,
  129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,233,72,
  184,237,237,252,255,208,255,249,73,137,221,73,129,197,239,49,201,129,201,
  239,72,49,210,65,137,141,233,73,137,149,233,73,139,190,233,72,184,237,237,
  252,255,208,73,137,198,76,137,252,247,76,137,252,238,72,184,237,237,252,255,
  208,73,137,197,73,129,252,253,239,15,133,245,73,186,237,237,252,233,245,249,
  65,139,141,233,73,139,149,233,255,137,139,233,72,137,147,233,252,233,245,
  249,73,137,221,73,129,197,239,49,201,129,201,239,72,49,210,65,137,141,233,
  73,137,149,233,73,139,190,233,72,184,237,237,252,255,208,73,137,198,76,137,
  252,247,76,137,252,238,72,184,237,237,252,255,208,73,137,197,73,129,252,253,
  239,15,133,245,73,186,237,237,252,233,245,249,255,65,139,141,233,73,139,149,
  233,137,139,233,72,137,147,233,249,73,252,255,194,249,255,73,137,222,73,129,
  198,239,65,139,134,233,129,252,248,239,15,132,245,65,139,134,233,129,252,
  248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,233,72,184,237,
  237,252,255,208,249,73,137,221,73,129,197,239,73,139,190,233,72,184,237,237,
  252,255,208,73,137,198,76,137,252,247,76,137,252,238,72,184,237,237,255,252,
  255,208,73,137,197,73,129,252,253,239,15,132,245,65,139,141,233,73,139,149,
  233,137,139,233,72,137,147,233,73,186,237,237,252,233,245,249,73,137,221,
  73,129,197,239,73,139,190,233,72,184,237,237,252,255,208,73,137,198,76,137,
  252,247,76,137,252,238,72,184,237,237,252,255,208,73,137,197,73,129,252,253,
  239,15,132,245,255,65,139,141,233,73,139,149,233,137,139,233,72,137,147,233,
  73,186,237,237,252,233,245,249,73,252,255,194,249,255,73,137,222,73,129,198,
  239,65,139,134,233,129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,
  76,137,144,233,72,184,237,237,252,255,208,249,73,137,222,73,129,198,239,65,
  139,134,233,129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,
  144,233,72,184,237,237,252,255,208,249,255,73,137,221,73,129,197,239,65,139,
  133,233,129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,
  233,72,184,237,237,252,255,208,249,73,139,134,233,102,72,15,110,192,73,139,
  133,233,102,72,15,110,200,102,15,46,193,15,147,208,36,1,60,0,15,132,245,73,
  186,237,237,252,233,245,249,73,252,255,194,249,255,73,137,222,73,129,198,
  239,73,137,221,73,129,197,239,73,137,220,73,129,196,239,65,139,134,233,129,
  252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,233,72,184,
  237,237,252,255,208,249,73,139,134,233,102,72,15,110,192,73,139,132,253,36,
  233,102,72,15,110,200,252,242,15,88,193,73,139,133,233,102,72,15,110,200,
  102,15,46,200,15,151,208,36,1,60,0,15,132,245,102,72,15,126,192,73,137,134,
  233,73,186,237,237,255,73,137,222,73,129,198,239,65,139,134,233,129,252,248,
  239,15,132,245,65,139,134,233,129,252,248,239,15,133,245,65,139,134,233,131,
  252,248,0,15,133,245,249,73,186,237,237,252,233,245,249,73,252,255,194,255,
  73,186,237,237,252,233,245,255,72,191,237,237,73,139,135,233,76,137,144,233,
  72,184,237,237,252,255,208,73,139,135,233,76,139,144,233,255,73,139,135,233,
  72,139,128,233,72,129,252,248,239,15,133,245,76,137,252,255,73,139,135,233,
  76,137,144,233,72,184,237,237,252,255,208,91,90,89,65,91,65,92,65,93,65,94,
  91,65,90,65,95,93,195,249,73,139,135,233,72,139,128,233,72,139,128,233,72,
  129,252,248,239,15,132,245,76,137,252,255,73,139,135,233,76,137,144,233,72,
  184,237,237,255,252,255,208,91,90,89,65,91,65,92,65,93,65,94,91,65,90,65,
  95,93,195,249,76,137,252,255,73,139,135,233,76,137,144,233,72,184,237,237,
  252,255,208,91,90,89,65,91,65,92,65,93,65,94,91,65,90,65,95,93,195,255,72,
  191,237,237,73,139,135,233,76,137,144,233,72,184,237,237,252,255,208,73,139,
  135,233,72,139,152,233,72,105,219,239,73,3,159,233,73,139,135,233,76,139,
  144,233,255
};

# 5 "./src/a2_jitx64.desc"

//|.globals _A2_VM_JIT_GLOB_
enum {
  _A2_VM_JIT_GLOB__MAX
};
# 7 "./src/a2_jitx64.desc"

//-----------------------------------------------------------------------
// the vm and a2_obj struct operation
//-----------------------------------------------------------------------
//|// a2_vm struct  map 
//|.type A2_VM, struct a2_vm, r15                   // struct a2_vm 
#define Dt1(_V) (int)(ptrdiff_t)&(((struct a2_vm *)0)_V)
# 13 "./src/a2_jitx64.desc"
//|.type SF_CIBASE, struct a2_obj, rbx              // the stack frame addr of call info
#define Dt2(_V) (int)(ptrdiff_t)&(((struct a2_obj *)0)_V)
# 14 "./src/a2_jitx64.desc"
//|.define CURR_PC, r10                             // the current pc
//|.type CURR_CI, struct vm_callinfo, rax
#define Dt3(_V) (int)(ptrdiff_t)&(((struct vm_callinfo *)0)_V)
# 16 "./src/a2_jitx64.desc"

// object map
//|.type R_OBJ1, struct a2_obj, r14 
#define Dt4(_V) (int)(ptrdiff_t)&(((struct a2_obj *)0)_V)
# 19 "./src/a2_jitx64.desc"
//|.type R_OBJ2, struct a2_obj, r13
#define Dt5(_V) (int)(ptrdiff_t)&(((struct a2_obj *)0)_V)
# 20 "./src/a2_jitx64.desc"
//|.type R_OBJ3, struct a2_obj, r12
#define Dt6(_V) (int)(ptrdiff_t)&(((struct a2_obj *)0)_V)
# 21 "./src/a2_jitx64.desc"
//|.type R_UV1, struct a2_upvalue, r11
#define Dt7(_V) (int)(ptrdiff_t)&(((struct a2_upvalue *)0)_V)
# 22 "./src/a2_jitx64.desc"
//|.type R_GCOBJ1, struct a2_gcobj, r11
#define Dt8(_V) (int)(ptrdiff_t)&(((struct a2_gcobj *)0)_V)
# 23 "./src/a2_jitx64.desc"
//|.type R_ARRAY, struct a2_array, r11
#define Dt9(_V) (int)(ptrdiff_t)&(((struct a2_array *)0)_V)
# 24 "./src/a2_jitx64.desc"

//|.define OBJ_SIZE, sizeof(struct a2_obj)
//|.define OBJ_STEP, 4

//|// set a2_vm addr
//|.macro set_vm, p
//|	mov64 A2_VM, (uintptr_t)p
//|.endmacro

//|.macro get_curr_ci
//| mov CURR_CI, A2_VM->call_chain
//|.endmacro

// get base stack frame
//|.macro get_base_sf
//| get_curr_ci
//| mov SF_CIBASE, CURR_CI->reg_stack.sf_idx
//| imul SF_CIBASE, OBJ_SIZE
//| add SF_CIBASE, A2_VM->stack_frame.sf_p
//|.endmacro

//|// get curr_pc
//|.macro get_pc
//| get_curr_ci
//| mov CURR_PC, CURR_CI[0].pc
//|.endmacro

 // inc curr_pc
//|.macro inc_pc
//| inc CURR_PC
//|.endmacro

// set pc
//|.macro set_pc, pc
//| mov64 CURR_PC, (uintptr_t)pc
//|.endmacro

// write curr_pc
//|.macro write_pc
//| get_curr_ci
//| mov CURR_CI[0].pc, CURR_PC
//|.endmacro


// get a a2_obj address with idx from stack frame
// the address will save R1 register
//|.macro sf_obj, idx, R1
//| mov R1, SF_CIBASE; add R1, idx*OBJ_SIZE
//|.endmacro

//|.macro sf_idx_obj, sf_idx, R1
//| mov R1, A2_VM->stack_frame.sf_p
//| add R1, sf_idx*OBJ_SIZE
//|.endmacro

//|// copy object
//|.macro copy_obj, D, S, R1, R2
//|	mov R1, S.type; mov64 R2, (uintptr_t)(S.value.point)
//|	mov D.type, R1; mov D.value.point, R2 
//|.endmacro
//|.macro copy_obj, D, S; copy_obj D, S, ecx, rdx; .endmacro

//|// copy register object
//|.macro copy_Robj, D, S, R1, R2
//| mov R1, S.type; mov R2, S.value.point
//| mov D.type, R1; mov D.value.point, R2 
//|.endmacro
//|.macro copy_Robj, D, S; copy_Robj D, S, ecx, rdx; .endmacro

//|// copy a2 object to stack frame by sf index
//|.macro obj2sf, idx, obj_p
//|	copy_obj SF_CIBASE[idx], (*obj_p)
//|.endmacro


//|.macro reg_obj2sf, idx, R1
//| copy_Robj SF_CIBASE[idx], R1[0]
//|.endmacro

//|.macro reg_obj2sf_idx, sf_idx, R1, R2
//| sf_idx_obj sf_idx, R2
//| copy_Robj R2[0], R1[0]
//|.endmacro

// set nil for a2 object
//|.macro nil4obj, R_OBJ
//| xor ecx, ecx; or ecx, A2_TNIL; xor rdx, rdx
//| mov R_OBJ[0].type, ecx; mov R_OBJ[0].value.point, rdx
//|.endmacro

// compare the a2_obj type
//|.macro cmp_type, R_OBJ, t
//| mov eax, R_OBJ->type
//| cmp eax, t
//|.endmacro


// get a value from stack frame or const stack
// the address will save R1 register
//|.macro get_value, idx, R1
//|| if(idx<0){ // the const value
//||   struct a2_obj* obj = a2_closure_const((curr_cls), (idx));
//|    mov64 R1, (uintptr_t)obj
//|| }else{  // the stack frame 
//|    sf_obj idx, R1
//|| }
//|.endmacro

// get a a2_obj address with key from global map
// the address will save R2 register
//|.macro get_envglobal, K, R2
//| mov rdi, A2_VM->env_p // set env_p
//| mov rsi, K
//| callp a2_get_envglobal
//| mov R2, rax
//|.endmacro

// set a a2_obj address with key and value from global map
//|.macro set_envglobal, K, V
//| mov rdi, A2_VM->env_p
//| mov rsi, K
//| mov rdx, V
//| callp a2_set_envglobal
//|.endmacro


// the gcobject to upvalue obj
//|.macro gcobj2upvalue, uv_obj, R1
//| mov64 rdi, (uintptr_t)uv_obj
//| callp a2_gcobj2upvalue
//| mov R1, rax
//|.endmacro


// call a function with a arg
//|.macro callp, addr
//| mov64  rax, (uintptr_t)addr
//| call   rax
//|.endmacro

//----------- dynasm operator function ------------
// dynasm struct 
#include <sys/mman.h>
#include "a2_jitcode.h"
static dasm_State *state = NULL;
size_t maxpc = 0;
struct {
  size_t* label_pc;
  size_t cap;
  size_t size;
}pc_stack;

size_t* lpc_chain = NULL;

#define Dst &state
#define DEF_PC_SIZE 64
#define DEF_RING_SIZE 32

static void a2_dynasm_new(dasm_State **state, const void *actionlist){
  pc_stack.cap = 0;
  pc_stack.size = DEF_PC_SIZE;
  pc_stack.label_pc = (size_t*)malloc(sizeof(size_t)*DEF_PC_SIZE);

  dasm_init(state, 1);
  dasm_setup(state, actionlist);
}

static void a2_dynasm_free(dasm_State **state){
  free(pc_stack.label_pc);
  pc_stack.cap = pc_stack.size = 0;
	dasm_free(state);
  maxpc = 0;
}

static size_t push_labelpc(size_t pc){
  if(pc_stack.cap >= pc_stack.size){
    pc_stack.size*=2;
    pc_stack.label_pc = (size_t*)realloc(pc_stack.label_pc, pc_stack.size);
  }

  pc_stack.label_pc[pc_stack.cap++] = pc;
  return pc;
}

static size_t pop_labelpc(){
  assert(pc_stack.cap>0);
  pc_stack.cap--;
  return pc_stack.label_pc[pc_stack.cap];
}

static dynasm_func a2_jitcode_new(dasm_State **state) {
  size_t size;
  a2_assert(dasm_link(state, &size), ==, DASM_S_OK);

  // Allocate memory readable and writable so we can
  // write the encoded instructions there.
  char *mem = (char*)mmap(NULL, size + sizeof(size_t),
		   PROT_READ | PROT_WRITE,
                   MAP_ANON | MAP_PRIVATE, -1, 0);
  assert(mem != MAP_FAILED);

  // Store length at the beginning of the region, so we
  // can free it without additional context.
  *(size_t*)mem = size;
  void *ret = mem + sizeof(size_t);

  dasm_encode(state, ret);

  // Adjust the memory permissions so it is executable
  // but no longer writable.
  a2_assert(mprotect(mem, size, PROT_EXEC | PROT_READ), ==, 0);
  return ret;
}

void a2_jitcode_free(dynasm_func code){
  void *mem = (char*)code - sizeof(size_t);
  a2_assert(munmap(mem, *(size_t*)mem), ==, 0);
}


//-----------------------------------------------------------------------
// dynasm operator function for test 
//-----------------------------------------------------------------------
void pt(void* p){
  printf("point = %p\n", p);
}

void look(){
  printf("look here!\n");
}

//-----------------------------------------------------------------------
// dynasm operator function for head 
//-----------------------------------------------------------------------
static void _per(struct a2_vm* vm_p){
  a2_dynasm_new(&state, actions);
  // init vm & stack frame
  //| push rbp
  //|//---------back register--------------
  //| push A2_VM
  //| push CURR_PC
  //| push SF_CIBASE
  //| push R_OBJ1
  //| push R_OBJ2
  //| push R_OBJ3
  //| push R_UV1
  //| push rcx
  //| push rdx
  //| push rbx  // the movaps is des address must 16 multiple.
  //| mov rbp, rsp
  //|//---------set vm register-------------
  //| set_vm, vm_p
  //| get_pc
  //| get_base_sf
  dasm_put(Dst, 0, (unsigned int)((uintptr_t)vm_p), (unsigned int)(((uintptr_t)vm_p)>>32), Dt1(->call_chain), Dt3([0].pc), Dt1(->call_chain), Dt3(->reg_stack.sf_idx), sizeof(struct a2_obj), Dt1(->stack_frame.sf_p));
# 278 "./src/a2_jitx64.desc"
}

//|.macro dynasm_ret, return_op
//|//---------recover register-----------
//| pop rbx
//| pop rdx
//| pop rcx
//| pop R_UV1
//| pop R_OBJ3
//| pop R_OBJ2
//| pop R_OBJ1
//| pop SF_CIBASE
//| pop CURR_PC
//| pop A2_VM
//| pop rbp
//|//---------write pc-----------------
//| return_op
//| ret
//|.endmacro


//|.macro _end_return_ax
//|
//|.endmacro


//|.macro dynasm_ret_ax
//| dynasm_ret _end_return_ax
//|.endmacro


static dynasm_func _end(struct a2_vm* vm_p){

  //| dynasm_ret_ax
  dasm_put(Dst, 49);
# 312 "./src/a2_jitx64.desc"

  // get dynasm func
  dynasm_func fptr = a2_jitcode_new(&state);
  a2_dynasm_free(&state);
  // fptr();
  // a2_jitcode_free(fptr);
  return fptr;
}

//-----------------------------------------------------------------------
// the load opcode 
//-----------------------------------------------------------------------
// load
static void _jit_load(struct a2_vm* vm_p, ir _ir){
	size_t idx = ir_ga(_ir);
	struct a2_obj* src_obj = a2_closure_const(curr_cls, ir_gbx(_ir));

  //| obj2sf idx, src_obj
  //| inc_pc
  dasm_put(Dst, 68, (*src_obj).type, (unsigned int)((uintptr_t)((*src_obj).value.point)), (unsigned int)(((uintptr_t)((*src_obj).value.point))>>32), Dt2([idx].type), Dt2([idx].value.point));
# 331 "./src/a2_jitx64.desc"
}

// load nil 
static void _jit_loadnil(struct a2_vm* vm_p, ir _ir){
  size_t begin_idx = ir_ga(_ir);
  size_t end_idx = begin_idx+ir_gbx(_ir);

  // add label
  dasm_growpc(&state, maxpc+=2);
  //|.define  SF_TP, R_OBJ1   // set stack frame pointer
  //|.define  END_TP, R_OBJ2
  //| mov END_TP, SF_CIBASE
  //| add END_TP, end_idx*OBJ_SIZE   
  //| mov SF_TP, SF_CIBASE
  //| add SF_TP, OBJ_SIZE*begin_idx  // begin ptr
  //|=>(maxpc-2):           // loop begin
  //| cmp SF_TP, END_TP
  //| je  =>(maxpc-1)       // if end, so break
  //| nil4obj SF_TP         // set nil for object 
  //| add SF_TP, OBJ_SIZE
  //| jmp =>(maxpc-2)       // next loop
  //|=>(maxpc-1): 
  //| inc_pc
  dasm_put(Dst, 86, end_idx*sizeof(struct a2_obj), sizeof(struct a2_obj)*begin_idx, (maxpc-2), (maxpc-1), A2_TNIL, Dt4([0].type), Dt4([0].value.point), sizeof(struct a2_obj), (maxpc-2), (maxpc-1));
# 354 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// the get /set global
//-----------------------------------------------------------------------
// get global
static void _jit_getglobal_error(struct a2_vm* vm_p, struct a2_obj* k){
  char _buf[64] = {0};
  a2_error(vm_p->env_p, e_vm_error, 
    "[vm error@%zd]: the global \'%s\' is not find.\n", 
    curr_line, 
    obj2str(k, _buf, sizeof(_buf)));
}

static void _jit_getglobal(struct a2_vm* vm_p, ir _ir){
  size_t des_idx = ir_ga(_ir);
  int idx = ir_gbx(_ir);

  dasm_growpc(&state, maxpc+=1);
  //| get_value idx, R_OBJ1      // get key obj
   if(idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, idx*sizeof(struct a2_obj));
   }
# 374 "./src/a2_jitx64.desc"
  //| get_envglobal R_OBJ1, R_OBJ2  // get global obj
  //| cmp R_OBJ2, 0              
  //| jne =>(maxpc-1)
  //| mov rdi, A2_VM           // not find the key 
  //| mov rsi, R_OBJ1
  //| write_pc 
  //| callp _jit_getglobal_error
  //|=>(maxpc-1):
  //| reg_obj2sf des_idx, R_OBJ2   // set global value
  //| inc_pc
  dasm_put(Dst, 150, Dt1(->env_p), (unsigned int)((uintptr_t)a2_get_envglobal), (unsigned int)(((uintptr_t)a2_get_envglobal)>>32), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_jit_getglobal_error), (unsigned int)(((uintptr_t)_jit_getglobal_error)>>32), (maxpc-1), Dt5([0].type), Dt5([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point));
# 384 "./src/a2_jitx64.desc"

}

// set global
static void _jit_setglobal(struct a2_vm* vm_p, ir _ir){
  int k_idx = ir_gb(_ir);
  int v_idx = ir_gc(_ir);
  size_t des_idx = ir_ga(_ir);

  //| get_value k_idx, R_OBJ1        // get key obj
   if(k_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (k_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, k_idx*sizeof(struct a2_obj));
   }
# 394 "./src/a2_jitx64.desc"
  //| get_value v_idx, R_OBJ2        // get value obj
   if(v_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (v_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, v_idx*sizeof(struct a2_obj));
   }
# 395 "./src/a2_jitx64.desc"
  //| set_envglobal R_OBJ1, R_OBJ2      // set global 
  //| reg_obj2sf des_idx, R_OBJ2  // copy to stack frame
  //| inc_pc
  dasm_put(Dst, 233, Dt1(->env_p), (unsigned int)((uintptr_t)a2_set_envglobal), (unsigned int)(((uintptr_t)a2_set_envglobal)>>32), Dt5([0].type), Dt5([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point));
# 398 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// the get / set upvalue 
//-----------------------------------------------------------------------
// get upvalue
static void _jit_getupvalue(struct a2_vm* vm_p, ir _ir){
  struct a2_upvalue* _uv = a2_closure_upvalue(curr_cls, ir_gbx(_ir));
  size_t des_idx = ir_ga(_ir);

  dasm_growpc(&state, maxpc+=2);
  //| mov eax, _uv->type
  //| cmp eax, uv_stack
  //| jne =>(maxpc-1)
  //| sf_idx_obj _uv->v.sf_idx, R_OBJ1
  //| reg_obj2sf des_idx, R_OBJ1      // internal copy from stack frame
  //| jmp =>(maxpc-2)
  //|=>(maxpc-1):
  //| cmp eax, uv_gc
  //| jne =>(maxpc-2) 
  //| gcobj2upvalue _uv->v.uv_obj, R_OBJ1
  dasm_put(Dst, 272, _uv->type, uv_stack, (maxpc-1), Dt1(->stack_frame.sf_p), _uv->v.sf_idx*sizeof(struct a2_obj), Dt4([0].type), Dt4([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point), (maxpc-2), (maxpc-1), uv_gc, (maxpc-2), (unsigned int)((uintptr_t)_uv->v.uv_obj), (unsigned int)(((uintptr_t)_uv->v.uv_obj)>>32), (unsigned int)((uintptr_t)a2_gcobj2upvalue), (unsigned int)(((uintptr_t)a2_gcobj2upvalue)>>32));
# 419 "./src/a2_jitx64.desc"
  //| reg_obj2sf des_idx, R_OBJ1      // copy to stack frame from upvalue
  //|=>(maxpc-2):
  //| inc_pc
  dasm_put(Dst, 324, Dt4([0].type), Dt4([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point), (maxpc-2));
# 422 "./src/a2_jitx64.desc"
}

// set upvalue
static void _jit_setupvalue(struct a2_vm* vm_p, ir _ir){
  struct a2_upvalue* _uv_d = a2_closure_upvalue(curr_cls, ir_ga(_ir));
  int idx = ir_gbx(_ir);

  dasm_growpc(&state, maxpc+=2);
  //| get_value idx, R_OBJ1  // get value
   if(idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, idx*sizeof(struct a2_obj));
   }
# 431 "./src/a2_jitx64.desc"
  //| mov eax, _uv_d->type
  //| cmp eax, uv_gc
  //| jne =>(maxpc-1)
  //| gcobj2upvalue _uv_d->v.uv_obj, R_OBJ2
  //| copy_Robj R_OBJ2[0], R_OBJ1[0]
  //| jmp =>(maxpc-2)
  //|=>(maxpc-1):
  //| cmp eax, uv_stack
  //| jne =>(maxpc-2)
  //| reg_obj2sf_idx _uv_d->v.sf_idx, R_OBJ1, R_OBJ2 // set stack frame value   
  dasm_put(Dst, 351, _uv_d->type, uv_gc, (maxpc-1), (unsigned int)((uintptr_t)_uv_d->v.uv_obj), (unsigned int)(((uintptr_t)_uv_d->v.uv_obj)>>32), (unsigned int)((uintptr_t)a2_gcobj2upvalue), (unsigned int)(((uintptr_t)a2_gcobj2upvalue)>>32), Dt4([0].type), Dt4([0].value.point), Dt5([0].type), Dt5([0].value.point), (maxpc-2), (maxpc-1), uv_stack, (maxpc-2), Dt1(->stack_frame.sf_p), _uv_d->v.sf_idx*sizeof(struct a2_obj));
# 441 "./src/a2_jitx64.desc"
  //|=>(maxpc-2):
  //| inc_pc
  dasm_put(Dst, 410, Dt4([0].type), Dt4([0].value.point), Dt5([0].type), Dt5([0].value.point), (maxpc-2));
# 443 "./src/a2_jitx64.desc"
}


//-----------------------------------------------------------------------
// the error list 
//-----------------------------------------------------------------------
static void _gv_type_error(struct a2_vm* vm_p){
  vm_error("the varable is not map or array.");
}
static void _gk_array_error(struct a2_vm* vm_p){
  vm_error("the key is must number at array.");
}
static void _gk_map_error(struct a2_vm* vm_p){
  vm_error("the key is must number or string at map.");
}
static void _gv_error(struct a2_vm* vm_p){
  vm_error("the key is overfllow.");
}
static void _opt_error(struct a2_vm* vm_p){
  vm_error("the varable is not number.");
}
static void _fp_error(struct a2_vm* vm_p){
  vm_error("the varable is not map or array.");
}
static void _for_step_error(struct a2_vm* vm_p){
  vm_error("the for's step is must number.");
}
static void _for_count_error(struct a2_vm* vm_p){
  vm_error("the for's count is must number.");
}
static void _for_idx_error(struct a2_vm* vm_p){
  vm_error("the index varable is must number.");
}

// call a function 
//|.macro dynasm_call, func
//| mov rdi, A2_VM
//| write_pc
//| callp func
//|.endmacro

//-----------------------------------------------------------------------
//  get / set value
//-----------------------------------------------------------------------
//|.macro obj2container, func, R_OBJ, R2
//| mov rdi, R_OBJ->value.obj
//| callp func
//| mov R2, rax
//|.endmacro

//|.macro obj2c, vv, R_OBJ, R2
//| mov R_GCOBJ1, R_OBJ->value.obj
//| mov R2, R_GCOBJ1[0].value.vv
//|.endmacro

//|.macro container_get, func, R_C, R_K, R2
//| mov rdi, R_C
//| mov rsi, R_K
//| callp func
//| mov R2, rax
//|.endmacro

//|.macro array_get, R_C, R_K, R2
//| cvttsd2si rax, R_K[0].value.number
//| cmp rax, 0
//| jl =>(maxpc-7) 
//| mov R_ARRAY, R_C
//| cmp rax, R_ARRAY->len
//| jge =>(maxpc-7)
//| mov R2, R_ARRAY->list
//| imul rax, OBJ_SIZE
//| shl rax, OBJ_STEP
//| add R2, rax
//| jmp =>(maxpc-8)
//|=>(maxpc-7):
//| xor R2, R2
//|=>(maxpc-8):
//|.endmacro

// get array with index
//|.macro get_array, R_C, R_K, R2
//| obj2c array, R_C, R_C
//| obj2container a2_gcobj2array, R_C, R_C
//| container_get a2_array_get, R_C, R_K, R2
//| array_get R_C, R_K, R2
//|.endmacro

// get map with index
//|.macro get_map, R_C, R_K, R2
//| obj2c map, R_C, R_C
//| obj2container a2_gcobj2map, R_C, R_C
//| container_get a2_map_query, R_C, R_K, R2
//|.endmacro

// next array with index
//|.macro next_array, R_C, R_K, R2
//| obj2container a2_gcobj2array, R_C, R_C
//| container_get a2_array_next, R_C, R_K, R2
//|.endmacro

// next map with index
//|.macro next_map, R_C, R_K, R2
//| obj2container a2_gcobj2map, R_C, R_C
//| container_get a2_map_next, R_C, R_K, R2
//|.endmacro

// the container operator
//|.macro container_oper, c_idx, k_idx, op_macro
//|| dasm_growpc(&state, maxpc+=8); 
//| sf_obj c_idx,   R_OBJ2       // get container
//| get_value  k_idx,  R_OBJ3       // get key
//| cmp_type R_OBJ2, A2_TARRAY      // if array
//| jne =>(maxpc-1)
//| cmp_type R_OBJ3, A2_TNUMBER 
//| je =>(maxpc-3)
//| dynasm_call _gk_array_error    // if key not number
//|=>(maxpc-3):
//| get_array R_OBJ2, R_OBJ3, R_OBJ1
//| cmp R_OBJ1, NULL
//| je =>(maxpc-4)
//| op_macro
//| jmp =>(maxpc-5)
//|=>(maxpc-1):
//| cmp_type R_OBJ2, A2_TMAP      // if map
//| jne =>(maxpc-2)
//| cmp_type R_OBJ3, A2_TNUMBER
//| je =>(maxpc-6)
//| cmp_type R_OBJ3, A2_TSTRING
//| je =>(maxpc-6)
//| dynasm_call _gk_map_error   // if key not number & string
//|=>(maxpc-6):
//| get_map R_OBJ2, R_OBJ3, R_OBJ1
//| cmp R_OBJ1, 0
//| je =>(maxpc-4)
//| op_macro
//| jmp =>(maxpc-5)
//|=>(maxpc-2):
//| dynasm_call _gv_type_error    // if the type not array and map 
//|=>(maxpc-4):
//| dynasm_call _gv_error // not get value
//|=>(maxpc-5):
//|.endmacro

static void _jit_getvalue(struct a2_vm* vm_p, ir _ir){
  size_t des_idx = ir_ga(_ir);
  size_t c_idx = ir_gb(_ir);
  int k_idx = ir_gc(_ir);

  //|.macro get_op
  //| reg_obj2sf des_idx, R_OBJ1
  //|.endmacro


  //| container_oper c_idx, k_idx, get_op
   dasm_growpc(&state, maxpc+=8); 
  dasm_put(Dst, 225, c_idx*sizeof(struct a2_obj));
   if(k_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (k_idx));
  dasm_put(Dst, 432, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 437, k_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 445, Dt5(->type), A2_TARRAY, (maxpc-1), Dt6(->type), A2_TNUMBER, (maxpc-3), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gk_array_error), (unsigned int)(((uintptr_t)_gk_array_error)>>32), (maxpc-3), Dt5(->value.obj), Dt8([0].value.array), Dt6([0].value.number), (maxpc-7), Dt9(->len), (maxpc-7));
  dasm_put(Dst, 526, Dt9(->list), (maxpc-8), (maxpc-7), (maxpc-8), NULL, (maxpc-4), Dt4([0].type), Dt4([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point), (maxpc-5), (maxpc-1), Dt5(->type), A2_TMAP, (maxpc-2), Dt6(->type));
  dasm_put(Dst, 591, A2_TNUMBER, (maxpc-6), Dt6(->type), A2_TSTRING, (maxpc-6), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gk_map_error), (unsigned int)(((uintptr_t)_gk_map_error)>>32), (maxpc-6), Dt5(->value.obj), Dt8([0].value.map), (unsigned int)((uintptr_t)a2_map_query), (unsigned int)(((uintptr_t)a2_map_query)>>32), (maxpc-4), Dt4([0].type), Dt4([0].value.point));
# 597 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 673, Dt2([des_idx].type), Dt2([des_idx].value.point), (maxpc-5), (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gv_type_error), (unsigned int)(((uintptr_t)_gv_type_error)>>32), (maxpc-4), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gv_error), (unsigned int)(((uintptr_t)_gv_error)>>32), (maxpc-5));
# 598 "./src/a2_jitx64.desc"
}

static void _jit_setvalue(struct a2_vm* vm_p, ir _ir){
  size_t c_idx = ir_ga(_ir);
  int k_idx = ir_gb(_ir);
  int v_idx = ir_gc(_ir);

  //|.macro set_op
  //| get_value v_idx, R_OBJ2
  //| copy_Robj R_OBJ1[0], R_OBJ2[0]
  //|.endmacro

  //| container_oper c_idx, k_idx, set_op
   dasm_growpc(&state, maxpc+=8); 
  dasm_put(Dst, 225, c_idx*sizeof(struct a2_obj));
   if(k_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (k_idx));
  dasm_put(Dst, 432, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 437, k_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 445, Dt5(->type), A2_TARRAY, (maxpc-1), Dt6(->type), A2_TNUMBER, (maxpc-3), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gk_array_error), (unsigned int)(((uintptr_t)_gk_array_error)>>32), (maxpc-3), Dt5(->value.obj), Dt8([0].value.array), Dt6([0].value.number), (maxpc-7), Dt9(->len), (maxpc-7));
  dasm_put(Dst, 729, Dt9(->list), (maxpc-8), (maxpc-7), (maxpc-8), NULL, (maxpc-4));
   if(v_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (v_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, v_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 758, Dt5([0].type), Dt5([0].value.point), Dt4([0].type), Dt4([0].value.point), (maxpc-5), (maxpc-1), Dt5(->type), A2_TMAP, (maxpc-2), Dt6(->type), A2_TNUMBER, (maxpc-6), Dt6(->type), A2_TSTRING, (maxpc-6), Dt1(->call_chain));
  dasm_put(Dst, 824, Dt3([0].pc), (unsigned int)((uintptr_t)_gk_map_error), (unsigned int)(((uintptr_t)_gk_map_error)>>32), (maxpc-6), Dt5(->value.obj), Dt8([0].value.map), (unsigned int)((uintptr_t)a2_map_query), (unsigned int)(((uintptr_t)a2_map_query)>>32), (maxpc-4));
   if(v_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (v_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, v_idx*sizeof(struct a2_obj));
   }
# 611 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 870, Dt5([0].type), Dt5([0].value.point), Dt4([0].type), Dt4([0].value.point), (maxpc-5), (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gv_type_error), (unsigned int)(((uintptr_t)_gv_type_error)>>32), (maxpc-4), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_gv_error), (unsigned int)(((uintptr_t)_gv_error)>>32), (maxpc-5));
# 612 "./src/a2_jitx64.desc"
}


//-----------------------------------------------------------------------
// arithmetical and > < >= <= operatior 
//-----------------------------------------------------------------------
//|.macro arithmetical_op, op_macro, tt
//||size_t des_idx = ir_ga(_ir);
//||int a_idx = ir_gb(_ir);
//||int b_idx = ir_gc(_ir);
//||dasm_growpc(&state, maxpc+=2);
//| get_value a_idx, R_OBJ1
//| get_value b_idx, R_OBJ2
//| cmp_type R_OBJ1, A2_TNUMBER 
//| jne =>(maxpc-1)
//| cmp_type R_OBJ2, A2_TNUMBER
//| jne =>(maxpc-1)                  // if obj1 or obj2 not number type
//| mov rax, R_OBJ1->value.number
//| movd xmm0, rax
//| mov rax, R_OBJ2->value.number
//| movd xmm1, rax
//| op_macro xmm0, xmm1
//| movd rax, xmm0
//| sf_obj des_idx, R_OBJ1       // get des obj ptr
//| mov ecx, tt
//| mov R_OBJ1[0].type, ecx
//| mov R_OBJ1[0].value.number, rax // set add value
//| jmp =>(maxpc-2)
//|=>(maxpc-1):
//| dynasm_call _opt_error
//|=>(maxpc-2):
//|.endmacro

// jit add 
static void _jit_add(struct a2_vm* vm_p, ir _ir){
  //|.macro add_op, X1, X2
  //| addsd X1, X2
  //|.endmacro

  //| arithmetical_op add_op, A2_TNUMBER
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 935, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TNUMBER, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 652 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 653 "./src/a2_jitx64.desc"
}

// jit sub 
static void _jit_sub(struct a2_vm* vm_p, ir _ir){
  //|.macro sub_op, X1, X2
  //| subsd X1, X2
  //|.endmacro

  //| arithmetical_op sub_op, A2_TNUMBER
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1023, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TNUMBER, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 662 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 663 "./src/a2_jitx64.desc"
}

// jit div
static void _jit_div(struct a2_vm* vm_p, ir _ir){
  //|.macro div_op, X1, X2
  //| divsd X1, X2
  //|.endmacro

  //| arithmetical_op div_op, A2_TNUMBER
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1111, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TNUMBER, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 672 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 673 "./src/a2_jitx64.desc"
}

// jit mul
static void _jit_mul(struct a2_vm* vm_p, ir _ir){
  //|.macro mul_op, X1, X2
  //| mulsd X1, X2
  //|.endmacro

  //| arithmetical_op mul_op, A2_TNUMBER
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1199, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TNUMBER, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 682 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 683 "./src/a2_jitx64.desc"
}

// jit big
static void _jit_big(struct a2_vm* vm_p, ir _ir){
  //|.macro big_op, X1, X2
  //| ucomisd X1, X2
  //| seta al
  //| and al, 1
  //| movzx rax, al
  //| movd X1, rax
  //|.endmacro

  //|  arithmetical_op big_op, A2_TBOOL
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1287, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TBOOL, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 696 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 697 "./src/a2_jitx64.desc"
} 

// jit lite
static void _jit_lite(struct a2_vm* vm_p, ir _ir){
  //|.macro lite_op, X1, X2
  //| ucomisd X2, X1
  //| seta al
  //| and al, 1
  //| movzx rax, al
  //| movd X1, rax
  //|.endmacro

  //|  arithmetical_op lite_op, A2_TBOOL
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1388, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TBOOL, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 710 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 711 "./src/a2_jitx64.desc"
}


// jit bige
static void _jit_bige(struct a2_vm* vm_p, ir _ir){
  //|.macro bige_op, X1, X2
  //| ucomisd X1, X2
  //| setae al
  //| and al, 1
  //| movzx rax, al
  //| movd X1, rax
  //|.endmacro

  //|  arithmetical_op bige_op, A2_TBOOL
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1489, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TBOOL, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 725 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 726 "./src/a2_jitx64.desc"
}


// jit litee
static void _jit_litee(struct a2_vm* vm_p, ir _ir){
  //|.macro litee_op, X1, X2
  //| ucomisd X2, X1
  //| setae al
  //| and al, 1
  //| movzx rax, al
  //| movd X1, rax
  //|.endmacro


  //|  arithmetical_op litee_op, A2_TBOOL
  size_t des_idx = ir_ga(_ir);
  int a_idx = ir_gb(_ir);
  int b_idx = ir_gc(_ir);
  dasm_growpc(&state, maxpc+=2);
   if(a_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (a_idx));
  dasm_put(Dst, 137, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 142, a_idx*sizeof(struct a2_obj));
   }
   if(b_idx<0){ // the const value
     struct a2_obj* obj = a2_closure_const((curr_cls), (b_idx));
  dasm_put(Dst, 220, (unsigned int)((uintptr_t)obj), (unsigned int)(((uintptr_t)obj)>>32));
   }else{  // the stack frame 
  dasm_put(Dst, 225, b_idx*sizeof(struct a2_obj));
   }
  dasm_put(Dst, 1590, Dt4(->type), A2_TNUMBER, (maxpc-1), Dt5(->type), A2_TNUMBER, (maxpc-1), Dt4(->value.number), Dt5(->value.number), des_idx*sizeof(struct a2_obj), A2_TBOOL, Dt4([0].type), Dt4([0].value.number), (maxpc-2), (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_opt_error), (unsigned int)(((uintptr_t)_opt_error)>>32));
# 741 "./src/a2_jitx64.desc"
  //| inc_pc
  dasm_put(Dst, 720, (maxpc-2));
# 742 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// jit move 
//-----------------------------------------------------------------------
static void _jit_move(struct a2_vm* vm_p, ir _ir){
  int des_idx = ir_ga(_ir);
  int src_idx = ir_gbx(_ir);

  //| sf_obj src_idx, R_OBJ1
  //| reg_obj2sf des_idx, R_OBJ1
  //| inc_pc
  dasm_put(Dst, 1691, src_idx*sizeof(struct a2_obj), Dt4([0].type), Dt4([0].value.point), Dt2([des_idx].type), Dt2([des_idx].value.point));
# 754 "./src/a2_jitx64.desc"
}


//-----------------------------------------------------------------------
// jit foreach 
//-----------------------------------------------------------------------
static size_t _jump_pc(struct a2_vm* vm_p, ir _ir){
  struct a2_obj* _oa = a2_closure_const(curr_cls, ir_gbx(_ir));
  assert(obj_t(_oa) == _A2_TADDR);
  return obj_vX(_oa, addr);  
}

static void _jit_foreachprep(struct a2_vm* vm_p, ir _ir){
  int c_idx = ir_ga(_ir)+2;
  int k_idx = ir_ga(_ir);
  int v_idx = ir_ga(_ir)+1;

  size_t j_pc = _jump_pc(vm_p, _ir);

  dasm_growpc(&state, maxpc+=7);
  //| sf_obj c_idx, R_OBJ1 // get the container value
  //| cmp_type R_OBJ1, A2_TMAP
  //| je =>(maxpc-1)
  //| cmp_type R_OBJ1, A2_TARRAY
  //| je =>(maxpc-2)
  //| dynasm_call _fp_error
  //|
  //| //set init varable
  dasm_put(Dst, 1718, c_idx*sizeof(struct a2_obj), Dt4(->type), A2_TMAP, (maxpc-1), Dt4(->type), A2_TARRAY, (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_fp_error), (unsigned int)(((uintptr_t)_fp_error)>>32));
# 782 "./src/a2_jitx64.desc"
  push_labelpc(maxpc);
  //|=>(maxpc-1):   // foreach map
  //| sf_obj k_idx, R_OBJ2
  //| nil4obj R_OBJ2  // set key is nil
  //| next_map R_OBJ1, R_OBJ2, R_OBJ2
  //| cmp R_OBJ2, NULL
  //| jne =>(maxpc-5)
  //| set_pc j_pc
  //| jmp =>(maxpc-3)
  //|=>(maxpc-5):
  //| reg_obj2sf v_idx, R_OBJ2
  dasm_put(Dst, 1767, (maxpc-1), k_idx*sizeof(struct a2_obj), A2_TNIL, Dt5([0].type), Dt5([0].value.point), Dt4(->value.obj), (unsigned int)((uintptr_t)a2_gcobj2map), (unsigned int)(((uintptr_t)a2_gcobj2map)>>32), (unsigned int)((uintptr_t)a2_map_next), (unsigned int)(((uintptr_t)a2_map_next)>>32), NULL, (maxpc-5), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (maxpc-3), (maxpc-5), Dt5([0].type), Dt5([0].value.point));
# 793 "./src/a2_jitx64.desc"
  //| jmp =>(maxpc-4)
  //|=>(maxpc-2):   // foreach array
  //| sf_obj k_idx, R_OBJ2
  //| nil4obj R_OBJ2  // set key is nil
  //| next_array R_OBJ1, R_OBJ2, R_OBJ2
  //| cmp R_OBJ2, NULL
  //| jne =>(maxpc-6)
  //| set_pc j_pc
  //| jmp =>(maxpc-3)
  //|=>(maxpc-6):
  //| reg_obj2sf v_idx, R_OBJ2
  dasm_put(Dst, 1848, Dt2([v_idx].type), Dt2([v_idx].value.point), (maxpc-4), (maxpc-2), k_idx*sizeof(struct a2_obj), A2_TNIL, Dt5([0].type), Dt5([0].value.point), Dt4(->value.obj), (unsigned int)((uintptr_t)a2_gcobj2array), (unsigned int)(((uintptr_t)a2_gcobj2array)>>32), (unsigned int)((uintptr_t)a2_array_next), (unsigned int)(((uintptr_t)a2_array_next)>>32), NULL, (maxpc-6), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (maxpc-3), (maxpc-6));
# 804 "./src/a2_jitx64.desc"
  //|=>(maxpc-4):   // next
  //| inc_pc
  //|=>(maxpc-7):
  dasm_put(Dst, 1931, Dt5([0].type), Dt5([0].value.point), Dt2([v_idx].type), Dt2([v_idx].value.point), (maxpc-4), (maxpc-7));
# 807 "./src/a2_jitx64.desc"
}

static  void _jit_foreachloop(struct a2_vm* vm_p, ir _ir){
  int c_idx = ir_ga(_ir)+2;
  int k_idx = ir_ga(_ir);
  int v_idx = ir_ga(_ir)+1;

  size_t j_pc = _jump_pc(vm_p, _ir);

  dasm_growpc(&state, maxpc+=3);
  size_t l_pc = pop_labelpc();
  //| sf_obj c_idx, R_OBJ1 // get the container value
  //| cmp_type R_OBJ1, A2_TMAP
  //| je =>(maxpc-1)
  //| cmp_type R_OBJ1, A2_TARRAY
  //| je =>(maxpc-2)
  //| dynasm_call _fp_error
  //|
  //| //set init varable
  //|
  //|=>(maxpc-1):   // foreach map
  //| sf_obj k_idx, R_OBJ2
  //| next_map R_OBJ1, R_OBJ2, R_OBJ2
  dasm_put(Dst, 1953, c_idx*sizeof(struct a2_obj), Dt4(->type), A2_TMAP, (maxpc-1), Dt4(->type), A2_TARRAY, (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_fp_error), (unsigned int)(((uintptr_t)_fp_error)>>32), (maxpc-1), k_idx*sizeof(struct a2_obj), Dt4(->value.obj), (unsigned int)((uintptr_t)a2_gcobj2map), (unsigned int)(((uintptr_t)a2_gcobj2map)>>32), (unsigned int)((uintptr_t)a2_map_next), (unsigned int)(((uintptr_t)a2_map_next)>>32));
# 830 "./src/a2_jitx64.desc"
  //| cmp R_OBJ2, NULL
  //| je =>(maxpc-3)
  //| reg_obj2sf v_idx, R_OBJ2
  //| set_pc j_pc
  //| jmp =>(l_pc-7)
  //|
  //|=>(maxpc-2):  // forech array
  //| sf_obj k_idx, R_OBJ2
  //| next_array R_OBJ1, R_OBJ2, R_OBJ2
  //| cmp R_OBJ2, NULL
  //| je =>(maxpc-3)
  //| reg_obj2sf v_idx, R_OBJ2
  dasm_put(Dst, 2036, NULL, (maxpc-3), Dt5([0].type), Dt5([0].value.point), Dt2([v_idx].type), Dt2([v_idx].value.point), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (l_pc-7), (maxpc-2), k_idx*sizeof(struct a2_obj), Dt4(->value.obj), (unsigned int)((uintptr_t)a2_gcobj2array), (unsigned int)(((uintptr_t)a2_gcobj2array)>>32), (unsigned int)((uintptr_t)a2_array_next), (unsigned int)(((uintptr_t)a2_array_next)>>32), NULL, (maxpc-3));
# 842 "./src/a2_jitx64.desc"
  //| set_pc j_pc
  //| jmp =>(l_pc-7)
  //|
  //|=>(maxpc-3): // next
  //| inc_pc
  //|=>(l_pc-3):
  dasm_put(Dst, 2121, Dt5([0].type), Dt5([0].value.point), Dt2([v_idx].type), Dt2([v_idx].value.point), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (l_pc-7), (maxpc-3), (l_pc-3));
# 848 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// jit for 
//-----------------------------------------------------------------------
static void _jit_forprep(struct a2_vm* vm_p, ir _ir){
  int _i = ir_ga(_ir);
  int _count = ir_ga(_ir)+1;
  int _step = ir_ga(_ir)+2;

  size_t j_pc = _jump_pc(vm_p, _ir);

  dasm_growpc(&state, maxpc+=6);
  push_labelpc(maxpc);
  //| sf_obj _step, R_OBJ1
  //| cmp_type R_OBJ1, A2_TNUMBER
  //| je =>(maxpc-1)
  //| dynasm_call _for_step_error // step error
  //|=>(maxpc-1):
  //| sf_obj _i, R_OBJ1
  //| cmp_type R_OBJ1, A2_TNUMBER
  //| je =>(maxpc-2)
  //| dynasm_call _for_idx_error // index error
  //|=>(maxpc-2):
  //| sf_obj _count, R_OBJ2
  dasm_put(Dst, 2150, _step*sizeof(struct a2_obj), Dt4(->type), A2_TNUMBER, (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_for_step_error), (unsigned int)(((uintptr_t)_for_step_error)>>32), (maxpc-1), _i*sizeof(struct a2_obj), Dt4(->type), A2_TNUMBER, (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_for_idx_error), (unsigned int)(((uintptr_t)_for_idx_error)>>32), (maxpc-2));
# 873 "./src/a2_jitx64.desc"
  //| cmp_type R_OBJ2, A2_TNUMBER
  //| je =>(maxpc-3)
  //| dynasm_call _for_count_error // count error
  //|=>(maxpc-3):
  //| mov rax, R_OBJ1[0].value.number
  //| movd xmm0, rax
  //| mov rax, R_OBJ2[0].value.number
  //| movd xmm1, rax
  //| ucomisd xmm0, xmm1
  //| setae al
  //| and al, 1
  //| cmp al, 0
  //| je =>(maxpc-4)
  //| set_pc j_pc
  //| jmp =>(maxpc-5) // jump for end 
  //|=>(maxpc-4):
  //| inc_pc
  //|=>(maxpc-6):
  //|
  dasm_put(Dst, 2227, _count*sizeof(struct a2_obj), Dt5(->type), A2_TNUMBER, (maxpc-3), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_for_count_error), (unsigned int)(((uintptr_t)_for_count_error)>>32), (maxpc-3), Dt4([0].value.number), Dt5([0].value.number), (maxpc-4), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (maxpc-5), (maxpc-4), (maxpc-6));
# 892 "./src/a2_jitx64.desc"
}

static void _jit_forloop(struct a2_vm* vm_p, ir _ir){
  int _i = ir_ga(_ir);
  int _count = ir_ga(_ir)+1;
  int _step = ir_ga(_ir)+2;

  size_t j_pc = _jump_pc(vm_p, _ir);
  size_t l_pc = pop_labelpc();

  dasm_growpc(&state, maxpc+=2);
  //| sf_obj _i, R_OBJ1
  //| sf_obj _count, R_OBJ2
  //| sf_obj _step, R_OBJ3
  //| cmp_type R_OBJ1, A2_TNUMBER
  //| je =>(maxpc-1)
  //| dynasm_call _for_idx_error // index error
  //|=>(maxpc-1):
  //| mov rax, R_OBJ1[0].value.number
  //| movd xmm0, rax
  //| mov rax, R_OBJ3[0].value.number
  //| movd xmm1, rax
  //| addsd xmm0, xmm1
  //| mov rax, R_OBJ2[0].value.number
  //| movd xmm1, rax
  //| ucomisd xmm1, xmm0
  //| seta al
  //| and al, 1
  //| cmp al, 0
  //| je =>(maxpc-2)
  //| movd rax, xmm0 
  //| mov R_OBJ1[0].value.number, rax
  //| set_pc j_pc
  //| jmp =>(l_pc-6)
  dasm_put(Dst, 2311, _i*sizeof(struct a2_obj), _count*sizeof(struct a2_obj), _step*sizeof(struct a2_obj), Dt4(->type), A2_TNUMBER, (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_for_idx_error), (unsigned int)(((uintptr_t)_for_idx_error)>>32), (maxpc-1), Dt4([0].value.number), Dt6([0].value.number), Dt5([0].value.number), (maxpc-2), Dt4([0].value.number), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32));
# 926 "./src/a2_jitx64.desc"
  //|=>(maxpc-2):
  //| inc_pc
  //|=>(l_pc-5):
  //|
  dasm_put(Dst, 2140, (l_pc-6), (maxpc-2), (l_pc-5));
# 930 "./src/a2_jitx64.desc"
}


//-----------------------------------------------------------------------
// test jump  
//-----------------------------------------------------------------------
static void _jit_test(struct a2_vm* vm_p, ir _ir){
  int v_idx = ir_ga(_ir);
  size_t j_pc = _jump_pc(vm_p, _ir);

  dasm_growpc(&state, maxpc+=3);

  if(lpc_chain[j_pc] == 0){
    lpc_chain[j_pc] = (maxpc-3+1);
  }

  //| sf_obj v_idx, R_OBJ1
  //| cmp_type R_OBJ1, A2_TNIL
  //| je =>(maxpc-1)
  //| cmp_type R_OBJ1, A2_TBOOL
  //| jne =>(maxpc-2)
  //| mov eax, R_OBJ1[0].value.uinteger
  //| cmp eax, 0
  //| jne =>(maxpc-2)
  //|=>(maxpc-1): // if false
  //| set_pc j_pc
  //| jmp =>(lpc_chain[j_pc]-1)
  //|=>(maxpc-2):
  //| inc_pc
  dasm_put(Dst, 2425, v_idx*sizeof(struct a2_obj), Dt4(->type), A2_TNIL, (maxpc-1), Dt4(->type), A2_TBOOL, (maxpc-2), Dt4([0].value.uinteger), (maxpc-2), (maxpc-1), (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (lpc_chain[j_pc]-1), (maxpc-2));
# 959 "./src/a2_jitx64.desc"
}
static void _jit_jump(struct a2_vm* vm_p, ir _ir){
  size_t j_pc = _jump_pc(vm_p, _ir);
  dasm_growpc(&state, maxpc+=1);
  if(lpc_chain[j_pc] == 0){
    lpc_chain[j_pc] = (maxpc-1+1);
  }

  //| set_pc j_pc
  //| jmp =>(lpc_chain[j_pc]-1)
  dasm_put(Dst, 2479, (unsigned int)((uintptr_t)j_pc), (unsigned int)(((uintptr_t)j_pc)>>32), (lpc_chain[j_pc]-1));
# 969 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// not jit opcode 
//-----------------------------------------------------------------------

typedef void(*inter_func)(struct a2_vm*);
static void _njit_func(struct a2_vm* vm_p, inter_func func){
  //| mov64 rdi, (uintptr_t)vm_p
  //| write_pc
  //| callp func   // call interpreter function
  //| get_pc
  dasm_put(Dst, 2487, (unsigned int)((uintptr_t)vm_p), (unsigned int)(((uintptr_t)vm_p)>>32), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)func), (unsigned int)(((uintptr_t)func)>>32), Dt1(->call_chain), Dt3([0].pc));
# 981 "./src/a2_jitx64.desc"
}

static void _njit_vm_call(struct a2_vm* vm_p){
  struct a2_obj* _func = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
  switch(obj_t(_func)){
    case A2_TCLOSURE:
      __vm_call_function(vm_p, _func);
      a2_vm_run(vm_p);
      break;
    case A2_TCFUNCTION:
      __vm_call_cfunction(vm_p, _func);
      break;
    default:
      vm_error("the varable is not function type.");
  }
}

static void _njit_return(struct a2_vm* vm_p){ 
  dasm_growpc(&state, maxpc+=2);
  //|// the closure is end
  //| mov CURR_CI, A2_VM->call_chain
  //| mov rax, CURR_CI->next  
  //| cmp rax, NULL
  //| jne =>(maxpc-1)
  //| dynasm_call callinfo_free
  //| dynasm_ret_ax
  //|=>(maxpc-1):
  //| // if return to a2 function
  //| mov CURR_CI, A2_VM->call_chain
  //| mov CURR_CI, CURR_CI->next
  //| mov rax, CURR_CI->cls
  //| cmp rax, NULL
  //| je =>(maxpc-2)
  //| dynasm_call __vm_return_function
  dasm_put(Dst, 2515, Dt1(->call_chain), Dt3(->next), NULL, (maxpc-1), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)callinfo_free), (unsigned int)(((uintptr_t)callinfo_free)>>32), (maxpc-1), Dt1(->call_chain), Dt3(->next), Dt3(->cls), NULL, (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)__vm_return_function), (unsigned int)(((uintptr_t)__vm_return_function)>>32));
# 1015 "./src/a2_jitx64.desc"
  //| dynasm_ret_ax
  //|=>(maxpc-2):
  //| dynasm_call __vm_return_cfunction
  //| dynasm_ret_ax
  dasm_put(Dst, 2606, (maxpc-2), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)__vm_return_cfunction), (unsigned int)(((uintptr_t)__vm_return_cfunction)>>32));
# 1019 "./src/a2_jitx64.desc"
}

static void _vm_or(struct a2_vm* vm_p){
  _vm_opl(||);
}

static void _vm_and(struct a2_vm* vm_p){
  _vm_opl(&&);
}

static void _vm_equ(struct a2_vm* vm_p){
  _vm_ope(==);
}

static void _vm_nequ(struct a2_vm* vm_p){
  _vm_ope(!=);
}

// or - not jit
#define _njit_or(vm_p)    _njit_func(vm_p, _vm_or)

// and - not jit
#define _njit_and(vm_p)    _njit_func(vm_p, _vm_and)

// equ - not jit
#define _njit_equ(vm_p)    _njit_func(vm_p, _vm_equ)

// nequ - not jit
#define _njit_nequ(vm_p)   _njit_func(vm_p, _vm_nequ)

// not - not jit
#define _njit_not(vm_p)    _njit_func(vm_p, _vm_not)

// neg - not jit
#define _njit_neg(vm_p)    _njit_func(vm_p, _vm_neg)

// cat - not jit
#define _njit_cat(vm_p)   _njit_func(vm_p, _vm_cat)

// set list - not jit
#define _njit_setlist(vm_p) _njit_func(vm_p, _vm_setlist)

// set map - not jit
#define _njit_setmap(vm_p) _njit_func(vm_p, _vm_setmap)

// new list - not jit 
#define _njit_newlist(vm_p) _njit_func(vm_p, _vm_newlist)

// new map - not jit
#define _njit_newmap(vm_p) _njit_func(vm_p, _vm_newmap)

// closure - not jit
#define _njit_closure(vm_p) _njit_func(vm_p, _vm_closure) 

// call - not jit
static void _njit_call(struct a2_vm* vm_p){
  //| mov64 rdi, (uintptr_t)vm_p
  //| write_pc
  //| callp _njit_vm_call   // call interpreter function
  //| get_base_sf
  //| get_pc
  dasm_put(Dst, 2666, (unsigned int)((uintptr_t)vm_p), (unsigned int)(((uintptr_t)vm_p)>>32), Dt1(->call_chain), Dt3([0].pc), (unsigned int)((uintptr_t)_njit_vm_call), (unsigned int)(((uintptr_t)_njit_vm_call)>>32), Dt1(->call_chain), Dt3(->reg_stack.sf_idx), sizeof(struct a2_obj), Dt1(->stack_frame.sf_p), Dt1(->call_chain), Dt3([0].pc));
# 1080 "./src/a2_jitx64.desc"
}

//-----------------------------------------------------------------------
// jit build, the treace is function  
//-----------------------------------------------------------------------
static dynasm_func _jit_build(struct a2_vm* vm_p){
  size_t i;

  size_t count = a2_closure_ircount(curr_cls);
  lpc_chain = (size_t*)calloc(count, sizeof(size_t));

  _per(vm_p);
  for(i=0; i<count; i++){
    ir _ir = a2_closure_ir(curr_cls, i);
    
    if(0!=lpc_chain[i]){
      //|=>(lpc_chain[i]-1):
      dasm_put(Dst, 1929, (lpc_chain[i]-1));
# 1097 "./src/a2_jitx64.desc"
    }

    switch(ir_gop(_ir)){
      case LOAD:
        _jit_load(vm_p, _ir);
        break;
      case LOADNIL:
        _jit_loadnil(vm_p, _ir);
        break;
      case GETGLOBAL:
        _jit_getglobal(vm_p, _ir);
        break;
      case SETGLOBAL:
        _jit_setglobal(vm_p, _ir);
        break;
      case GETUPVALUE:
        _jit_getupvalue(vm_p, _ir);
        break;
      case SETUPVALUE:
        _jit_setupvalue(vm_p, _ir);
        break;
      case NEWLIST:
        _njit_newlist(vm_p);
        break;
      case NEWMAP:
        _njit_newmap(vm_p);
        break;
      case GETVALUE:
        _jit_getvalue(vm_p, _ir);
        break;
      case SETVALUE:
        _jit_setvalue(vm_p, _ir);
        break;
      case SETLIST:
        _njit_setlist(vm_p);
        break;
      case SETMAP:
        _njit_setmap(vm_p);
        break;
      case CLOSURE:
        _njit_closure(vm_p);
        break;
      case TEST:
        _jit_test(vm_p, _ir);
        break;
      case JUMP:
        _jit_jump(vm_p, _ir);
        break;
      case MOVE:
        _jit_move(vm_p, _ir);
        break;
      case ADD:
        _jit_add(vm_p, _ir);
        break;
      case SUB:
        _jit_sub(vm_p, _ir);
        break;
      case DIV:
        _jit_div(vm_p, _ir);
        break;
      case MUL:
        _jit_mul(vm_p, _ir);
        break;
      case OR:
        _njit_or(vm_p);
        break;
      case AND:
        _njit_and(vm_p);
        break;
      case BIG:
        _jit_big(vm_p, _ir);
        break;
      case LITE:
        _jit_lite(vm_p, _ir);
        break;
      case BIGE:
        _jit_bige(vm_p, _ir);
        break;
      case LITEE:
        _jit_litee(vm_p, _ir);
        break;
      case EQU:
        _njit_equ(vm_p);
        break;
      case NEQU:
        _njit_nequ(vm_p);
        break;
      case NOT:
        _njit_not(vm_p);
        break;
      case NEG:
        _njit_neg(vm_p);
        break;
      case CAT:
        _njit_cat(vm_p);
        break;
      case FOREACHLOOP:
        _jit_foreachloop(vm_p, _ir);
        break;
      case FOREACHPREP:
        _jit_foreachprep(vm_p, _ir);
        break;
      case FORPREP:
        _jit_forprep(vm_p, _ir);
        break;
      case FORLOOP:
        _jit_forloop(vm_p, _ir);
        break;
      case CALL:
        _njit_call(vm_p);
        break;
      case RETURN:
        _njit_return(vm_p);
        break;
      default:
        assert(0);
    }
  }

  free(lpc_chain);
  dynasm_func asm_func = _end(vm_p);
  return asm_func;
}



