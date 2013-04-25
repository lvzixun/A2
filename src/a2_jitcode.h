#ifndef _A2_JIT_CODE_H_
#define _A2_JIT_CODE_H_

typedef int(*dynasm_func)();
void a2_jitcode_free(dynasm_func code);

#endif