#ifndef _A2_CONF_H_
#define _A2_CONF_H_
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#undef malloc
#undef free
#undef calloc
#undef realloc

typedef unsigned char byte;
typedef byte uchar;

typedef int32_t  int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef double	 a2_number;

#define A2_API	 

 // for jitm only suport x64 unlix
//#define A2_JIT

// for mingw32 
#ifndef _MINGW32_
	#define _sf snprintf
#else
	#define _sf _snprintf
#endif

// 32bits
#if __SIZEOF_POINTER__ == 4
	#define NaN_Trick 
#endif


#ifdef _DEBUG_
	#define a2_assert(exp, op, v)  assert( (exp) op (v) )
	#define malloc(size) a2_malloc(size, __FILE__, __LINE__)
	#define free(p)   a2_free(p)
	#define realloc  a2_realloc
	#define calloc(c, size) a2_calloc(c, size, __FILE__, __LINE__)
	
	void* a2_realloc(void* p, size_t size);
	void* a2_malloc(size_t size,  char* file, int line);
	void  a2_free(void* p);
	void* a2_calloc(size_t count, size_t size, char* file, int line);
#else
	#undef assert
	#define assert(...) ((void)0)
	#define a2_assert(exp, op, v) exp
	#define malloc a2_malloc
	#define free  a2_free
	#define realloc a2_realloc
	#define calloc a2_calloc

	void* a2_malloc(size_t size);
	void* a2_calloc(size_t count, size_t size);
	void* a2_realloc(void* p, size_t size);
	void  a2_free(void* p);
#endif

#define atonum(s) (a2_number)atof(s)

#define a2_true 0
#define a2_fail !a2_true

#define check_null(p, r) do{if((p)==NULL) return (r);}while(0)

void mem_print();
#endif

