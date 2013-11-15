#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"
#include <string.h>
#include "a2_io.h"

#define MAX_IO_BUFFER 1024

struct a2_io{
	FILE* fp;
	size_t size;

	size_t seek;
	size_t len;
	size_t rsize;
	byte* buf;

	struct a2_env* env_p;
};

#define _is_end(p, n)		((p)->rsize+(p)->seek+(n)>=(p)->len)
#define is_end(p)   		_is_end(p, 0)
#define is_load(p, c)		(((p)->seek+(c))>=(p)->size)&&(!_is_end(p, c))

static void _a2_io_load(struct a2_io* io_p);

struct a2_io*  a2_io_open(struct a2_env* env_p, const char* file_name){
	struct a2_io* ret = NULL;
	FILE* fp = NULL;
	check_null(file_name, NULL);
	fp  = fopen(file_name, "rb");
	if(fp == NULL) {
		a2_error(env_p, e_io_error, "[io error]: the file\"%s\" is null!\n", file_name);
		return NULL;
	}
	ret = (struct a2_io*)malloc(sizeof(*ret));
	ret->env_p = env_p;
	ret->size = MAX_IO_BUFFER;
	ret->buf = (byte*)malloc(sizeof(byte)*MAX_IO_BUFFER);

	fseek(fp, 0L, SEEK_END);
	ret->len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	ret->fp = fp;
	ret->seek = 0;
	ret->rsize = 0;
	size_t len = fread(ret->buf, sizeof(byte), ret->size, ret->fp);
	if(len<=0)
		a2_error(env_p, e_io_error, "io error.\n");
	return ret;
}

void a2_io_close(struct a2_io* io_p){
	assert(io_p);

	fclose(io_p->fp);
	free(io_p->buf);
	free(io_p);
}


struct a2_io* a2_io_openS(const char*  str, size_t len){
	assert(str);
	assert(len>0);
	struct a2_io* ret = (struct a2_io*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));

	ret->buf = (byte*)str;
	ret->size = ret->len = len;
	return ret;
}

void a2_io_closeS(struct a2_io* io_p){
	assert(io_p);
	free(io_p);
}

// load objects from  fp
static void _a2_io_load(struct a2_io* io_p){
	memcpy(io_p->buf, &(io_p->buf[io_p->seek]), io_p->size-io_p->seek);
	size_t len= fread( &(io_p->buf[io_p->size-io_p->seek]), sizeof(byte), io_p->seek, io_p->fp);
	if(len<=0)
		a2_error(io_p->env_p, e_io_error, "io error.\n");
	
	io_p->rsize += io_p->seek;
	io_p->seek = io_p->size-io_p->seek;
}

// read a char from io buf
inline char a2_io_readchar(struct a2_io* io_p){
	assert(io_p);
	if(is_end(io_p)) 
		return '\0';
	if(is_load(io_p, 0)) 
		_a2_io_load(io_p);
	
	return io_p->buf[io_p->seek++];
}

// current char	 
inline char a2_io_atchar(struct a2_io* io_p){
	assert(io_p);
	return io_p->buf[io_p->seek];
}

// match n char
inline char a2_io_matchchar(struct a2_io* io_p, int n){
	assert(io_p);
	if(_is_end(io_p, n))
		return '\0';
	else if (n<=0 && ((-n)<=io_p->seek) )
		return io_p->buf[io_p->seek+n];
	else if( n>0 && n<io_p->size){
		if(is_load(io_p, n))
			_a2_io_load(io_p);
		return io_p->buf[io_p->seek+n];
	}

	return '\0';
}

// 
inline int a2_io_end(struct a2_io* io_p){
	assert(io_p);
	return is_end(io_p);
}

