#include "../a2_map.h"
#include "../a2_conf.h"
#include "../a2_string.h"
#include <stdio.h>

static char*  rand_key(){
	char buf[64] = {'\0'};
	int num = rand();

	sprintf(buf, "key_%d", num);
	return a2_string_new(buf);
}

int main(int argc, char const *argv[])
{
	struct a2_map*  map_p = a2_map_new();
	size_t i =0;
	struct a2_obj k, v = {0};
	struct a2_kv kv = {
		&k, &v
	};

	for(i=0; i<10; i++){
		k = a2_string2obj(rand_key());
		if(a2_map_add(map_p, &kv) == a2_fail)
			a2_obj_free(&k);
	}

	k = a2_string2obj( a2_string_new("key_168aaa07"));
	struct a2_obj* kp = NULL;
	if( (kp=a2_map_query(map_p, &k))==NULL ){
		printf("query key is error!\n");
	}else{
		printf("query = ");
		obj_dump(kp);
		printf("\n");
	}
	a2_obj_free(&k);

	k = a2_number2obj(12345);
	if(a2_map_add(map_p, &kv)==a2_fail){
		printf("add key is error!\n");
		a2_obj_free(&k);
	}
	printf("\ndump begin----\n");
	// dump 
	map_dump(map_p);

	k = a2_string2obj(a2_string_new("key_16807"));
	if(a2_map_add(map_p, &kv)==a2_fail){
		a2_obj_free(&k);
	}

	a2_map_free(map_p);
	mem_print();
	return 0;
}