#include <string.h>
#include "a2_conf.h"
#include "a2_map.h"
#include "a2_obj.h"
#include "a2_gc.h"
#include <stdio.h>

struct a2_slot{
	struct a2_obj key;
	struct a2_obj value;

	size_t hash;
	size_t next;
};

struct a2_map{
	size_t  size;
	size_t	cap;
	struct a2_slot* slot_p;
};

#define DEFAULT_MAP_LEN		32
#define is_nil(v) (obj_t(&(v))==_A2_TNULL)

inline size_t calc_hash(byte* name, size_t len)
{
	size_t h = len;
	size_t step = (len>>5)+1;
	size_t i;
	for (i=len; i>=step; i-=step)
		h = h ^ ((h<<5)+(h>>2)+(size_t)name[i-1]);
	return h;
}

struct a2_map* a2_map_new(){
	struct a2_map* ret = (struct a2_map*)malloc(sizeof(*ret));
	ret->size = DEFAULT_MAP_LEN;
	ret->cap = 0;
	ret->slot_p = (struct a2_slot*)malloc(DEFAULT_MAP_LEN*sizeof(struct a2_slot));
	int i;
	for(i=0; i<DEFAULT_MAP_LEN; i++){
		obj_setX( &(ret->slot_p[i].key), _A2_TNULL, obj, NULL);
		ret->slot_p[i].next = 0;
	}
	return ret;
}


void a2_map_free(struct a2_map* map_p){
	assert(map_p);
	
	free(map_p->slot_p);
	free(map_p);
}

void a2_map_clear(struct a2_map* map_p){
	size_t i;
	for(i=0; i<map_p->size; i++){
		a2_obj_free(&map_p->slot_p[i].key);
		a2_obj_free(&map_p->slot_p[i].value);
	}
	
	memset(map_p->slot_p, 0, sizeof(struct a2_slot)*map_p->size);
	map_p->cap = 0;
}

inline size_t a2_map_len(struct a2_map* map_p){
	assert(map_p);
	return map_p->cap;
}

static void map_resize(struct a2_map* map_p){
	size_t i, new_size = map_p->size<<1, hash;
	struct a2_slot* new_slot = (struct a2_slot*)calloc(new_size, sizeof(*new_slot));
	for(i=0; i<map_p->size; i++){
		hash = map_p->slot_p[i].hash % new_size;
		if(is_nil(new_slot[hash].key)){
			new_slot[hash].hash = map_p->slot_p[i].hash;
			new_slot[hash].key = map_p->slot_p[i].key;
			new_slot[hash].value = map_p->slot_p[i].value;
		}else{
			size_t emp = hash;
			for( ; !is_nil(new_slot[emp].key); )
				emp = (emp+1)%new_size;

				new_slot[emp].key = map_p->slot_p[i].key;
				new_slot[emp].hash = map_p->slot_p[i].hash;
				new_slot[emp].value = map_p->slot_p[i].value;
				new_slot[emp].next = new_slot[hash].next;
				new_slot[hash].next = emp+1;
		}
	}

	free(map_p->slot_p);
	map_p->size = new_size;
	map_p->slot_p = new_slot;
}

static struct a2_obj* _a2_map_query(struct a2_map* map_p, struct a2_obj* key, struct a2_obj** _key, size_t* slot){
	size_t hash_full, hash;

	hash_full = calc_hash( a2_obj_bytes(key), a2_obj_size(key));
	hash = hash_full % map_p->size;

	for(; !is_nil(map_p->slot_p[hash].key);){
		if( map_p->slot_p[hash].hash==hash_full && a2_obj_cmp(&map_p->slot_p[hash].key, key)==a2_true){
			*_key = &(map_p->slot_p[hash].key);
			if(slot)
				*slot = hash;
			return &(map_p->slot_p[hash].value);
		}
		if(map_p->slot_p[hash].next==0)
			return NULL;
		hash = map_p->slot_p[hash].next-1;
	}
	return NULL;
}

static inline struct a2_obj* _map_dump(struct a2_map* map_p, size_t begin, struct a2_obj* key){
	size_t i;
	for(i=begin; i<map_p->size; i++){
		if(!is_nil(map_p->slot_p[i].key)){
			*key = map_p->slot_p[i].key;
			return &(map_p->slot_p[i].value);
		}
	}
	return NULL;
}

inline struct a2_obj* a2_map_next(struct a2_map* map_p, struct a2_obj* key){
	assert(map_p && key);
	// begin 
	if(obj_t(key)==A2_TNIL)
		return _map_dump(map_p, 0, key);
	else{
		struct a2_obj* kp = NULL;
		size_t slot = 0;
		if(_a2_map_query(map_p, key, &kp, &slot)){
			return _map_dump(map_p, slot+1, key);
		}else
			return NULL;
	}

}


inline struct a2_obj* a2_map_set(struct a2_map* map_p, struct a2_obj* key, struct a2_obj* value){
	struct a2_obj* ret = a2_map_query(map_p, key);
	if(ret==NULL){
		struct a2_kv kv = {
			key, value
		};
		a2_map_add(map_p, &kv);
		return value;
	}
	*ret = *value;
	return value;
}


inline struct a2_obj* a2_map_query(struct a2_map* map_p, struct a2_obj* key){
	struct a2_obj* kp = NULL;
	return _a2_map_query(map_p, key, &kp, NULL);
}

int a2_map_add(struct a2_map* map_p, struct a2_kv* kv){
	size_t hash, sh;
	assert(map_p);
	assert(kv);
	assert(kv->key);

	hash = calc_hash( a2_obj_bytes(kv->key), a2_obj_size(kv->key));
	if(map_p->cap >= map_p->size)
		map_resize(map_p);
	sh = hash % map_p->size;
	if( is_nil(map_p->slot_p[sh].key)){
		map_p->slot_p[sh].key = *(kv->key);
		map_p->slot_p[sh].hash = hash;
		map_p->slot_p[sh].value = *(kv->vp);
	}else{
		size_t emp=sh, idx=sh;
		for( ;; ){
			if(map_p->slot_p[idx].hash == hash &&  a2_obj_cmp(&map_p->slot_p[idx].key, kv->key)==a2_true){
				assert(0);
				return a2_fail;
			}
			if(map_p->slot_p[idx].next==0)
				break;
			idx = map_p->slot_p[idx].next-1;
		}

		for( ; !is_nil(map_p->slot_p[emp].key); )
			emp=(emp+1)%map_p->size;

		map_p->slot_p[emp].hash = hash;
		map_p->slot_p[emp].key = *(kv->key);
		map_p->slot_p[emp].value = *(kv->vp);
		map_p->slot_p[emp].next = map_p->slot_p[sh].next;
		map_p->slot_p[sh].next = emp+1;
	}
	map_p->cap++;
	return a2_true;
}

int a2_map_del(struct a2_map* map_p, struct a2_obj* key){
	assert(map_p);
	assert(key);
	struct a2_obj* kp = NULL;
	struct a2_obj* vp = _a2_map_query(map_p, key, &kp, NULL);
	if(vp==NULL)
		return a2_fail;
	else{
		a2_obj_free(vp);
		a2_obj_free(kp);
		return a2_true;
	}
}


// test dump
void map_dump(struct a2_map* map_p){
	size_t i =0;
	for(i=0; i<map_p->size; i++){
		printf("k= ");
		obj_dump(&(map_p->slot_p[i].key));
		printf("%s", "  v = ");
		obj_dump(&(map_p->slot_p[i].value));
		printf("\n");
	}
}	

