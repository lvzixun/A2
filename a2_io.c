#include <stdio.h>
#include "a2_conf.h"
#include "a2_error.h"

#define MAX_IO_BUFFER 1024

struct a2_io{
	FILE* fp;
	size_t seek;
	size_t len;
	int step;
	byte buf[1];
};

#define is_end(p)   ((((p)->step)*MAX_IO_BUFFER)+(p)->seek>=(p)->len)
#define is_load(p)	((p)->seek==MAX_IO_BUFFER)&&(!is_end(p))

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
	ret->step = -1;
	_a2_io_load(ret);
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
	size_t len = fread(io_p->buf, sizeof(byte), MAX_IO_BUFFER, io_p->fp);
	if(len)
		(io_p->step)++;
	io_p->seek = 0;
}

// read a char from io buf
char a2_io_readchar(struct a2_io* io_p){
	check_null(io_p, '\0');
	if(is_end(io_p)) 
		return '\0';
	if(is_load(io_p)) 
		_a2_io_load(io_p);
	
	return io_p->buf[io_p->seek++];
}

int a2_io_end(struct a2_io* io_p){
	check_null(io_p, a2_true);
	return is_end(io_p);
}