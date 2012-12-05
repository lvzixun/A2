#ifndef _A2_CONF_H_
#define _A2_CONF_H_
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#undef malloc
#undef free

typedef unsigned char byte;
typedef byte uchar;

typedef int32_t  int32;
typedef uint32_t uint32;
typedef double	 a2_number;

#define a2_assert assert
#define malloc a2_malloc
#define free   a2_free

#define a2_true 0
#define a2_fail !a2_true

#define check_null(p, r) do{if((p)==NULL) return (r);}while(0)

void* a2_malloc(size_t size);
void  a2_free(void* p);
void mem_print();
#endif