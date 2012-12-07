#include "../a2_io.h"
#include "../a2_conf.h"
#include <stdio.h>


int main(void){
	struct a2_io*  p = a2_io_open("tfile");
	printf("----file--- p= %p----\n", p);
	while(!a2_io_end(p)){
		printf("%c", a2_io_readchar(p));
	}

	a2_io_close(p);
	mem_print();
	return 0;
}