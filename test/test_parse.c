#include "../src/a2_conf.h"
#include "../src/a2_parse.h"
#include "../src/a2_env.h"
#include "../src/a2_lex.h"
#include "../src/a2_io.h"
#include "../src/a2_error.h"
#include <stdio.h>

static void dump_token(struct a2_token* tc, size_t len){
	size_t i;
	printf("-----token list------\n");
	for(i=0; i<len;i++){
		print_token(&tc[i]);
	}
	printf("---------------------\n");
}

int main(int argc, char const *argv[])
{
	struct a2_env* env = a2_env_new(NULL);
	struct a2_lex* lex_p = a2_envlex(env);
	struct a2_parse* parse_p = a2_envparse(env);
	struct a2_io *io_p = a2_io_open(env, "parse.a2");
	if(!io_p)
		a2_error(env, e_io_error, "file is null.\n");
	size_t token_len =0;
	struct a2_token* token_chain = a2_lex_read(lex_p, io_p, &token_len);

	dump_token(token_chain, token_len);
	size_t root = parse_run(parse_p, token_chain, token_len);
	dump_node(parse_p, root);

	a2_io_close(io_p);
	a2_env_free(env);
	mem_print();
	return 0;
}
