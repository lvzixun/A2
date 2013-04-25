/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.3.0, DynASM x64 version 1.3.0
** DO NOT EDIT! The original file is in "a2_jitX86.desc".
*/
#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"

#if DASM_VERSION != 10300
#error "Version mismatch between DynASM and included encoding engine"
#endif

# 1 "a2_jitX86.desc"
// the A2 JIT for x64 CPU.
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>


struct a2_obj{
	int type;
	size_t value;	
};

struct a2_vm{
	void* env_p;	
	struct a2_obj* sf_p;
};


//|.arch x64
//|.actionlist actions
static const unsigned char actions[23] = {
  72,184,237,237,72,139,152,233,185,237,72,199,194,237,137,139,233,72,137,147,
  233,195,255
};

# 22 "a2_jitX86.desc"
//|
//|// a2_vm struct  map 
//|.type A2_VM, struct a2_vm,  rax // struct a2_vm 
#define Dt1(_V) (int)(ptrdiff_t)&(((struct a2_vm *)0)_V)
# 25 "a2_jitX86.desc"
//|.type SF_P, struct a2_obj, rbx  // the stack frame addr
#define Dt2(_V) (int)(ptrdiff_t)&(((struct a2_obj *)0)_V)
# 26 "a2_jitX86.desc"
//|
//|// set a2_vm addr
//|.macro set_vm, p
//|	mov64 A2_VM, (uintptr_t)p
//|.endmacro
//|
//|// get stack frame head addr
//|.macro get_sf
//|	mov SF_P, A2_VM->sf_p
//|.endmacro
//|
//|// copy object
//|.macro copy_obj, D, S, R1, R2
//|	mov R1, S.type; mov R2, S.value
//|	mov D.type, R1; mov D.value, R2 
//|.endmacro
//|.macro copy_obj, D, S; copy_obj D, S, ecx, rdx; .endmacro
//|
//|// copy a2 object to stack frame by sf index
//|.macro obj2sf, sf_idx, obj_p
//|	copy_obj SF_P[sf_idx], (*obj_p)
//|.endmacro
//|
//|.macro callp, addr
//|  mov64  rax, (uintptr_t)addr
//|  call   rax
//|.endmacro

void initjit(dasm_State **state, const void *actionlist) {
  dasm_init(state, 1);
  dasm_setup(state, actionlist);
}

void tp(void* p){
	printf("point = %p\n", p);
}

void *jitcode(dasm_State **state) {
  size_t size;
  int dasm_status = dasm_link(state, &size);
  assert(dasm_status == DASM_S_OK);

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
  dasm_free(state);

  // Adjust the memory permissions so it is executable
  // but no longer writable.
  int success = mprotect(mem, size, PROT_EXEC | PROT_READ);
  assert(success == 0);
  return ret;
}

void free_jitcode(void *code) {
  void *mem = (char*)code - sizeof(size_t);
  int status = munmap(mem, *(size_t*)mem);
  assert(status == 0);
}

typedef void(*func)();
#define Dst &state

void print(struct a2_obj* obj){
	printf("type = %d value = %zd\n", obj->type, obj->value);
}

int main(void){
	struct a2_vm vm = {0};
  struct a2_obj _sf_p[3] = {0};
  vm.sf_p = _sf_p;
	dasm_State *state;
	initjit(&state, actions);

	size_t sf_idx = 0;
	struct a2_obj src_obj = {
		1, 22
	};	

	tp(vm.sf_p);
	//| set_vm &vm
	//| get_sf 
	//| obj2sf sf_idx, (&src_obj)
	//| ret
	dasm_put(Dst, 0, (unsigned int)((uintptr_t)&vm), (unsigned int)(((uintptr_t)&vm)>>32), Dt1(->sf_p), (*(&src_obj)).type, (*(&src_obj)).value, Dt2([sf_idx].type), Dt2([sf_idx].value));
# 120 "a2_jitX86.desc"

	func fptr = (func)jitcode(&state);
	
	print(vm.sf_p);
	fptr();
	print(vm.sf_p);

	free_jitcode((void*)fptr);

	return 0;
}



