#ifndef _A2_OBJ_H_
#define _A2_OBJ_H_
#include "a2_conf.h"
#include "a2_gc.h"
#include "a2.h"

typedef union {
		struct a2_gcobj* obj;
		a2_number number;
		a2_cfunction cfunction;
		void* point;
		uint32 uinteger; 
		size_t addr;
}a2_value;


#ifndef NaN_Trick
	struct a2_obj{
		int type;
		a2_value value;
	};
#else
	union  nan_u{
		struct a2_gcobj* obj;
		a2_cfunction cfunction;
		void* point;
		uint32 uinteger; 
		size_t addr;
	};

	struct a2_obj{
		union {
			// only lite endian
			struct {
				union nan_u _v;
				int _t;
			} _m;
			double number;
		} __v;
	};
#endif

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
	A2_TCFUNCTION,
	A2_TBOOL,

	//private type
	_A2_TUINTEGER,
	_A2_TADDR,
	_A2_TUPVALUE
};



#ifndef NaN_Trick
	#define obj_t(p)	((p)->type)
	#define obj_vX(p, vn)	((p)->value.vn)
	#define obj_vNum(p)		obj_vX(p, number)
	#define obj_setX(p, t, vn, v)	( (p)->type=(t), (p)->value.vn=(v) )
	#define obj_setNum(p, n)	obj_setX(p, A2_TNUMBER, number, n)
#else
	#define NN_MARK		0x7FF7A500
	#define NN_MASK		0x7FFFFF00
	#define POS_TYPE	(32)
	#define is_num(p)	((((p)->__v._m._t) & NN_MASK)!=NN_MARK)
	#define ot(p)		((p)->__v._m._t)
	#define obj_t(p)	( (is_num(p))?(A2_TNUMBER):(ot(p) & 0XFF) )	
	#define obj_vX(p, vn)	( (p)->__v._m._v.vn )
	#define obj_vNum(p)		( (p)->__v.number )
	#define obj_setX(p, t, vn, v)	( (p)->__v._m._t=(NN_MARK|(t)), (p)->__v._m._v.vn=(v) )
	#define obj_setNum(p, n)	((p)->__v.number=(n))
#endif

#define is_gcobj(obj)	(obj_t(obj)== A2_TMAP || obj_t(obj)==A2_TARRAY || \
						obj_t(obj)==A2_TCLOSURE || obj_t(obj)==A2_TSTRING )


size_t a2_obj_size(struct a2_obj* obj_p);
byte* a2_obj_bytes(struct a2_obj* obj_p);
int a2_obj_cmp(struct a2_obj* obj1, struct a2_obj* obj2);
const char* a2_type2string(int type);

struct a2_obj  a2_string2obj(char* a2_s);
struct a2_obj  a2_number2obj(a2_number num);
struct a2_obj  a2_point2obj(void* p);
struct a2_obj a2_uinteger2obj(uint32 v);
struct a2_obj a2_bool2obj(int t);
struct a2_obj a2_nil2obj();
struct a2_obj a2_addr2obj(size_t addr);
struct a2_obj a2_cfunction2obj(a2_cfunction func);

void a2_obj_free(struct a2_obj* obj_p);

inline char* obj2str(struct a2_obj* obj, char* buf, size_t len);

// test obj dump
void obj_dump(struct a2_obj* obj);

#endif

