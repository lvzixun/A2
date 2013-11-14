#include "a2_obj.h"
#include "a2_string.h"
#include "a2_closure.h"
#include <string.h>
#include <stdio.h>

typedef void(* _obj_free)(struct a2_obj*);

static const char* type_info[] = {
	"_null",
	
	// public type
	"nil",
	"string",
	"number",
	"point",
	"map",
	"array",
	"closure",
	"cfunction",
	"bool",

	//private type
	"_uinteger",
	"_addr",
	"_upvalue"
};

// a2_string to obj
struct a2_obj  a2_string2obj(char* a2_s){
	assert(a2_s);
	struct a2_obj ret;
	obj_setX(&ret, A2_TSTRING, obj, a2_string2gcobj(a2_s));
	return ret;
}

// a2_number to obj
struct a2_obj a2_number2obj(a2_number num){
	struct a2_obj ret;
	obj_setNum(&ret, num);
	return ret;
}

// void* point to obj
struct a2_obj  a2_point2obj(void* p){
	struct a2_obj ret;
	obj_setX(&ret, A2_TPOINT, point, p);
	return ret;
}

// uint32 v to obj
struct a2_obj a2_uinteger2obj(uint32 v){
	struct a2_obj ret;
	obj_setX(&ret, _A2_TUINTEGER, uinteger, v);
	return ret;
}

// bool varable to object
struct a2_obj a2_bool2obj(int t){
	struct a2_obj ret;
	obj_setX(&ret, A2_TBOOL, uinteger, (!(t==0)));
	return ret;
}

// nil varable to object
struct a2_obj a2_nil2obj(){
	struct a2_obj ret;
	obj_setX(&ret, A2_TNIL, point, NULL);
	return ret;
}

// cfunction to object
struct a2_obj a2_cfunction2obj(a2_cfunction func){
	assert(func);
	struct a2_obj ret;
	obj_setX(&ret, A2_TCFUNCTION, cfunction, func);
	return ret;
}

// addr to object
struct a2_obj a2_addr2obj(size_t addr){
	struct a2_obj ret;
	obj_setX(&ret, _A2_TADDR, addr, addr);
	return ret;
}

// get data size from a2_obj
size_t a2_obj_size(struct a2_obj* obj_p){
	assert(obj_p);
	switch(obj_t(obj_p)){
		case A2_TSTRING:
			return   a2_string_len(a2_gcobj2string(obj_vX(obj_p, obj)));
		case A2_TNUMBER:
			return sizeof(a2_number);
		case A2_TNIL:
			return 0;
		case A2_TBOOL:
			return sizeof(obj_vX(obj_p, uinteger));
		case _A2_TADDR:
			return sizeof(obj_vX(obj_p, addr));
		default:
			assert(0);
			return 0;
	}
}

byte* a2_obj_bytes(struct a2_obj* obj_p){
	assert(obj_p);
	switch(obj_t(obj_p)){
		case A2_TSTRING:
			return  (byte*)(a2_gcobj2string(obj_vX(obj_p, obj)));
		case A2_TNUMBER:
			return (byte*)(&obj_vNum(obj_p));
		case A2_TNIL:
			return NULL;
		case A2_TBOOL:
			return (byte*)(&obj_vX(obj_p, uinteger));
		case _A2_TADDR:
			return (byte*)(&obj_vX(obj_p, addr));
		default:
			assert(0);
			return NULL;
	}
}

int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2){
	return ((obj_t(obj1) == obj_t(obj2)) && 
			a2_obj_size(obj1)==a2_obj_size(obj2) && 
			memcmp(a2_obj_bytes(obj1), a2_obj_bytes(obj2), a2_obj_size(obj1))==0)?(a2_true):(a2_fail); 
}

const char* a2_type2string(int type){
	assert(type>=0 && type<sizeof(type_info)/sizeof(type_info[0]));
	return type_info[type];
}

inline char* obj2str(struct a2_obj* obj, char* buf, size_t len){
	assert(obj);
	switch(obj_t(obj)){
		case A2_TNUMBER:
			snprintf(buf, len, "%.14g", obj_vNum(obj));
			return buf;
		case A2_TSTRING:
			return a2_gcobj2string(obj_vX(obj, obj));
		case A2_TBOOL:
			snprintf(buf, len, "%s", (obj_vX(obj, uinteger))?("true"):("false"));
			return buf;
		case A2_TNIL:
			return "nil";
		case _A2_TADDR:
			_sf(buf, len, "[%zd]", obj_vX(obj, addr));
			return buf;
		case A2_TCLOSURE:
			snprintf(buf, len, "closure:%p", a2_gcobj2closure(obj_vX(obj, obj)));
			return buf;
		case A2_TARRAY:
			snprintf(buf, len, "array:%p", a2_gcobj2array(obj_vX(obj, obj)));
			return buf;
		case A2_TMAP:
			snprintf(buf, len, "map:%p", a2_gcobj2map(obj_vX(obj, obj)));
			return buf;
		default:
			assert(0);
	}
	return NULL;
}

// test obj dump
void obj_dump(struct a2_obj* obj){
	switch(obj_t(obj)){
		case A2_TSTRING:
			printf("%s", a2_gcobj2string(obj_vX(obj, obj)));
			break;
		case A2_TNUMBER:
			printf("%.14g", obj_vNum(obj));
			break;
		default:
			printf("<null> type: %d\n", obj_t(obj));
			break;
	}
}
