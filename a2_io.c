#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"
#include <string.h>

#define MAX_IO_BUFFER 1024

struct a2_io{
	FILE* fp;
	size_t seek;
	size_t len;
	size_t rsize;
	byte buf[1];
};

#define is_end(p)   ((p)->rsize+(p)->seek>=(p)->len)
#define is_load(p, c)	(((p)->seek+(c))>=MAX_IO_BUFFER)&&(!is_end(p))

static void _a2_io_load(struct a2_io* io_p);

struct a2_io*  a2_io_open(const char* file_name){
	struct a2_io* ret = NULL;
	FILE* fp = NULL;
	check_null(file_name, NULL);
	fp  = fopen(file_name, "rb");
	if(fp == NULL) {
		a2_error("[io error]: the file is null!\n");
		return NULL;
	}
	ret = (struct a2_io*)malloc(sizeof(*ret)+MAX_IO_BUFFER);
	fseek(fp, 0L, SEEK_END);
	ret->len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	ret->fp = fp;
	ret->seek = 0;
	ret->rsize = 0;
	fread(ret->buf, sizeof(byte), MAX_IO_BUFFER, ret->fp);
	return ret;
}

void a2_io_close(struct a2_io* io_p){
	if(io_p==NULL)
		return;

	fclose(io_p->fp);
	free(io_p);
}

// load objects from  fp
static void _a2_io_load(struct a2_io* io_p){
	memcpy(io_p->buf, &(io_p->buf[io_p->seek]), MAX_IO_BUFFER-io_p->seek);
	size_t len = fread( &(io_p->buf[MAX_IO_BUFFER-io_p->seek]), sizeof(byte), io_p->seek, io_p->fp);
	if(len<=0)
		a2_error("io error.");
	
	io_p->rsize += io_p->seek;
	io_p->seek = MAX_IO_BUFFER-io_p->seek;
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
	if(is_end(io_p))
		return '\0';
	else if (n<=0 && ((-n)<=io_p->seek) )
		return io_p->buf[io_p->seek+n];
	else if( n>0 && n<MAX_IO_BUFFER){
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

