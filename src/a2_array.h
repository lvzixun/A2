#ifndef _A2_ARRAY_H_
#define _A2_ARRAY_H_
#include "a2_conf.h"

struct a2_array{
	size_t len;
	size_t size;
	struct a2_obj* list;
};

struct a2_obj;

struct a2_array* a2_array_new();
void a2_array_free(struct a2_array* array_p);
inline struct a2_obj* a2_array_add(struct a2_array* array_p, struct a2_obj* obj);
inline struct a2_obj* a2_array_get(struct a2_array* array_p, struct a2_obj* k);
inline struct a2_obj* a2_array_next(struct a2_array* array_p, struct a2_obj* k);
inline size_t a2_array_len(struct a2_array* array_p);

#endif

