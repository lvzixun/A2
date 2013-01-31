#include "../a2_lex.h"
#include "../a2_gc.h"
#include "../a2_io.h"
#include <stdio.h>
#include "../a2_env.h"

static char ops[4] = {'\0'};

static char* op2s(uint32 op){
	int i=0;
	char c=0;
	c = (op&0xff0000)>>16;
	if(c)
		ops[i++]=c;
	c=(op&0xff00)>>8;
	if(c)
		ops[i++]=c;
	c=(op&0xff);
	if(c)
		ops[i++]=c;
	ops[i] = '\0';
	return ops;
}

int main(int argc, char const *argv[])
{
	static char*  _lk[] = {
		"tk_key",			// keyWord     like: function if else ..
		"tk_ide",			// identifier   var
		"tk_string",		// string       abcdefg
		"tk_number",		// number       12345
		"tk_end",			//  /n:       	
		"tk_op", 			// operate      like: + - * / ( ) [ ]
		"tk_args",			// variable parameters ...
		"tk_strcat" 		// string aappend ..
	};

	size_t i, len = 0;
	struct a2_io* io_p = a2_io_open("lex.a2");
	struct a2_env* env_p = a2_env_new(NULL);
	struct a2_lex* lex_p = a2_lex_open(env_p);
	struct a2_token* tp = NULL;

	tp = a2_lex_read(lex_p, io_p, &len);
	printf("lex read end!\n");
	for(i=0; i<len; i++){
		printf("[line:%zd]tk = %s  op =\" %s \" \n", tp[i].line, _lk[tt2tk(tp[i].tt)], op2s(tt2op(tp[i].tt)));
		if(tt2tk(tp[i].tt)==tk_ide || tt2tk(tp[i].tt)==tk_key || tt2tk(tp[i].tt)==tk_string){
			printf("v = %s %p", a2_gcobj2string(tp[i].v.obj), a2_gcobj2string(tp[i].v.obj));
		}else if(tt2tk(tp[i].tt)==tk_number){
			printf("v=%lf", tp[i].v.number);
		}
		printf("\n\n");
	}

	a2_lex_close(lex_p);
	a2_env_free(env_p);
	a2_io_close(io_p);
	mem_print();	
	return 0;
}