#include "../a2_env.h"
#include "../a2_lex.h"
#include "../a2_parse.h"
#include "../a2_closure.h"
#include "../a2_array.h"
#include "../a2_io.h"
#include "../a2_ir.h"
#include <stdio.h>


int main(int argc, char const *argv[])
{
	struct a2_env* env = a2_env_new(NULL);
	
	struct a2_lex* lex = a2_envlex(env);
	struct a2_parse* parse = a2_envparse(env);
	struct a2_io* io = a2_io_open("vm.a2");
	struct a2_ir* ir = a2_envir(env);
	size_t len=0;
	struct a2_token* token = a2_lex_read(lex, io, &len);
	int i;
	printf("----token-----\n");
	for(i=0; i<len; i++){
		print_token(&token[i]);
	}
	printf("----end-------\n");

	a2_parse_run(parse, token, len);

	// dump ir
	dump_ir(ir);

	a2_io_close(io);

	a2_env_free(env);
	
	mem_print();
	return 0;
}
