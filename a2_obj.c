#include "a2_obj.h"
#include "a2_string.h"
#include "a2_closure.h"
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

// bool varable to object
struct a2_obj a2_bool2obj(int t){
	struct a2_obj ret;
	ret.type = A2_TBOOL;
	ret.value.uinteger = (!(t==0));
	return ret;
}

// nil varable to object
struct a2_obj a2_nil2obj(){
	struct a2_obj ret;
	ret.type = A2_TNIL;
	ret.value.point = NULL;
	return ret;
}

// cfunction to object
struct a2_obj a2_cfunction2obj(a2_cfunction func){
	assert(func);
	struct a2_obj ret;
	ret.type = A2_TCFUNCTION;
	ret.value.cfunction = func;
	return ret;
}

// addr to object
struct a2_obj a2_addr2obj(size_t addr){
	struct a2_obj ret;
	ret.type = _A2_TADDR;
	ret.value.addr = addr;
	return ret;
}

void a2_obj_free(struct a2_obj* obj_p){
	assert(obj_p && obj_p->type <=4);
	if(obj_free_func[obj_p->type])
		obj_free_func[obj_p->type](obj_p);
	obj_p->type = _A2_TNULL;
}

// get data size from a2_obj
size_t a2_obj_size(struct a2_obj* obj_p){
	assert(obj_p);
	switch(obj_p->type){
		case A2_TSTRING:
			return   a2_string_len(a2_gcobj2string(obj_p->value.obj));
		case A2_TNUMBER:
			return sizeof(a2_number);
		case A2_TNIL:
			return 0;
		case A2_TBOOL:
			return sizeof(obj_p->value.uinteger);
		case _A2_TADDR:
			return sizeof(obj_p->value.addr);
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
		case A2_TNIL:
			return NULL;
		case A2_TBOOL:
			return (byte*)(&(obj_p->value.uinteger));
		case _A2_TADDR:
			return (byte*)(&(obj_p->value.addr));
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
		case A2_TBOOL:
			snprintf(buf, len, "%s", (obj->value.uinteger)?("true"):("false"));
			return buf;
		case A2_TNIL:
			return "nil";
		case _A2_TADDR:
			snprintf(buf, len, "[%lu]", obj->value.addr);
			return buf;
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
