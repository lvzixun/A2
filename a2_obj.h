#ifndef _A2_OBJ_H_
#define _A2_OBJ_H_
#include "a2_conf.h"
#include "a2_gc.h"

typedef union {
		struct a2_gcobj* obj;
		a2_number number;
		void* point;
		uint32 uinteger; 
		size_t addr;
}a2_value;

struct a2_obj{
	int type;
	a2_value value;
};

enum{
	// invalid object
	_A2_TNULL,
	
	// public type
	A2_TNIL = 1,
	A2_TSTRING,
	A2_TNUMBER,
	A2_TPOINT,
	A2_TMAP,
	A2_TARRAY,
	A2_TCLOSURE,
	A2_TFUNCTION,
	A2_TBOOL,

	//private type
	_A2_TUINTEGER,
	_A2_TADDR
};

#define str_obj(obj_p)  (a2_gcobj2string((obj_p)->value.obj))
#define num_obj(obj_p)  ((obj_p)->value.number)

size_t a2_obj_size(struct a2_obj* obj_p);
byte* a2_obj_bytes(struct a2_obj* obj_p);
int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2);

struct a2_obj  a2_string2obj(char* a2_s);
struct a2_obj  a2_number2obj(a2_number num);
struct a2_obj  a2_point2obj(void* p);
struct a2_obj a2_uinteger2obj(uint32 v);
struct a2_obj a2_bool2obj(int t);
struct a2_obj a2_nil2obj();
struct a2_obj a2_addr2obj(size_t addr);

void a2_obj_free(struct a2_obj* obj_p);

inline char* obj2str(struct a2_obj* obj, char* buf, size_t len);

// test obj dump
void obj_dump(struct a2_obj* obj);

#endif