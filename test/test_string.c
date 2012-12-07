#include "../a2_conf.h"
#include "../a2_string.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	char* str = "Linux is a clone of the operating system Unix, written from scratch by Linus Torvalds with assistance from a loosely-knit team of hackers across the Net. It aims towards POSIX and Single UNIX Specification compliance.It has all the features you would expect in a modern fully-fledged Unix,including true multitasking, virtual memory, shared libraries, demand loading, shared copy-on-write executables, proper memory management, and multistack networking including IPv4 and IPv6. It is distributed under the GNU General Public License - see theaccompanying COPYING file for more details.";
	char* a2_s = a2_string_new(str);

	printf("str = %s\n", a2_s);
	if(!strcmp(str, a2_s))
		printf("not equ!\n");
	a2_string_free(a2_s);
	mem_print();
	return 0;
}