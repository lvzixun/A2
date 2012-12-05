#include "a2_conf.h"
#include <string.h>

struct a2_map{

};

/*
static size_t calc_hash(const char* name)
{
	size_t len = strlen(name);
	size_t h = len;
	size_t step = (len>>5)+1;
	size_t i;
	for (i=len; i>=step; i-=step)
		h = h ^ ((h<<5)+(h>>2)+(size_t)name[i-1]);
	return h;
}*/