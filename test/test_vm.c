
#include "../a2.h"
#include "../a2_libutil.h"




int main(int argc, char const *argv[])
{
	struct a2_state* as = a2_open();
	a2_openutil(as);

	a2_loadfile(as, "vm.a2");

	a2_close(as);
//	mem_print();
	return 0;
}
