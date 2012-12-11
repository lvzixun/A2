#ifndef _A2_OBJ_H_
#define _A2_OBJ_H_
#include "a2_conf.h"
#include "a2_gc.h"

struct a2_obj{
	int type;
	union {
		struct a2_gcobj* obj;
		a2_number number;
	}value;
};

enum{
	A2_TSNIL,
	A2_TSTRING,
	A2_TNUMBER
};

size_t a2_obj_size(struct a2_obj* obj_p);
byte* a2_obj_bytes(struct a2_obj* obj_p);
int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2);

struct a2_obj  a2_string2obj(char* a2_s);
struct a2_obj  a2_number2obj(a2_number num);
void a2_obj_free(struct a2_obj* obj_p);

#endif