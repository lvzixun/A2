#include "a2_obj.h"
#include "a2_string.h"
#include <string.h>
#include <stdio.h>

typedef void(* _obj_free)(struct a2_obj*);

static void _obj_strfree(struct a2_obj* obj_p);

static _obj_free obj_free_func[] = {
	NULL,
	_obj_strfree,  // string
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,  // closure
	NULL,

	NULL
};


// a2_string to obj
struct a2_obj  a2_string2obj(char* a2_s){
	assert(a2_s);
	struct a2_obj ret;
	ret.type = A2_TSTRING;
	ret.value.obj = a2_string2gcobj(a2_s);
	return ret;
}

// a2_number to obj
struct a2_obj a2_number2obj(a2_number num){
	struct a2_obj ret;
	ret.type = A2_TNUMBER;
	ret.value.number = num;
	return ret;
}

// void* point to obj
struct a2_obj  a2_point2obj(void* p){
	struct a2_obj ret;
	ret.type = A2_TPOINT;
	ret.value.point = p;
	return ret;
}

// uint32 v to obj
struct a2_obj a2_uinteger2obj(uint32 v){
	struct a2_obj ret;
	ret.type = _A2_TUINTEGER;
	ret.value.uinteger = v;
	return ret;
}

void a2_obj_free(struct a2_obj* obj_p){
	assert(obj_p && obj_p->type <=4);
	if(obj_free_func[obj_p->type])
		obj_free_func[obj_p->type](obj_p);
	obj_p->type = A2_TSNIL;
}

// get data size from a2_obj
size_t a2_obj_size(struct a2_obj* obj_p){
	assert(obj_p);
	switch(obj_p->type){
		case A2_TSTRING:
			return   a2_string_len(a2_gcobj2string(obj_p->value.obj));
		case A2_TNUMBER:
			return sizeof(a2_number);
		default:
			return 0;
	}
}

byte* a2_obj_bytes(struct a2_obj* obj_p){
	assert(obj_p);
	switch(obj_p->type){
		case A2_TSTRING:
			return  (byte*)(a2_gcobj2string(obj_p->value.obj));
		case A2_TNUMBER:
			return (byte*)(&(obj_p->value.number));
		default:
			return NULL;
	}
}

static void _obj_strfree(struct a2_obj* obj_p){
	a2_gcobj_stringfree(obj_p->value.obj);
}

int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2){
	return ((obj1->type == obj2->type) && 
			a2_obj_size(obj1)==a2_obj_size(obj2) && 
			memcmp(a2_obj_bytes(obj1), a2_obj_bytes(obj2), a2_obj_size(obj1))==0)?(a2_true):(a2_fail); 
}

inline char* obj2str(struct a2_obj* obj, char* buf, size_t len){
	assert(obj);
	switch(obj->type){
		case A2_TNUMBER:
			snprintf(buf, len, "%.14g", obj->value.number);
			return buf;
		case A2_TSTRING:
			return a2_gcobj2string(obj->value.obj);
		default:
			assert(0);
	}
}

// test obj dump
void obj_dump(struct a2_obj* obj){
	switch(obj->type){
		case A2_TSTRING:
			printf("%s", str_obj(obj));
			break;
		case A2_TNUMBER:
			printf("%.14g", num_obj(obj));
			break;
		default:
			printf("<null>");
			break;
	}
}
