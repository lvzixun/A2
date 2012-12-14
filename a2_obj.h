#ifndef _A2_OBJ_H_
#define _A2_OBJ_H_
#include "a2_conf.h"
#include "a2_gc.h"

typedef union {
		struct a2_gcobj* obj;
		a2_number number;
		void* point;
}a2_value;

struct a2_obj{
	int type;
	a2_value value;
};

enum{
	A2_TSNIL,
	A2_TSTRING,
	A2_TNUMBER,
	A2_TPOINT
};

#define str_obj(obj_p)  (a2_gcobj2string((obj_p)->value.obj))
#define num_obj(obj_p)  ((obj_p)->value.number)

size_t a2_obj_size(struct a2_obj* obj_p);
byte* a2_obj_bytes(struct a2_obj* obj_p);
int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2);

struct a2_obj  a2_string2obj(char* a2_s);
struct a2_obj  a2_number2obj(a2_number num);
struct a2_obj  a2_point2obj(void* p);
void a2_obj_free(struct a2_obj* obj_p);


// test obj dump
void obj_dump(struct a2_obj* obj);

#endif