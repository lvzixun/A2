#ifndef _A2_MAP_H_
#define _A2_MAP_H_
#include "a2_conf.h"
#include "a2_obj.h"

struct a2_kv{
	struct a2_obj* key;
	struct a2_obj* vp;
};

struct a2_map;

struct a2_map* a2_map_new();
void a2_map_free(struct a2_map* map_p);

int a2_map_add(struct a2_map* map_p, struct a2_kv* kv);
int a2_map_del(struct a2_map* map_p, struct a2_obj* key);
inline struct a2_obj* a2_map_query(struct a2_map* map_p, struct a2_obj* key);
#endif