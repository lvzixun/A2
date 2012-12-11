#include <stdio.h>
#include <string.h>

static char cmask[0xff] = {'\0'};

static void print(){
	int i;
	char ts[4] = {'\'', '\0', '\'', '\0'};
	for(i=0; i<0xff; i++){
		if(i%51 == 0)
			printf("\n");

		char* p = ts;
		switch(cmask[i]){
			case '\'':
				p = "'\\''"; break;
			case '\t':
				p = "'\\t'"; break;
			case '\n':
				p = "'\\n'"; break;
		}
		printf("%s, ", (cmask[i])?(ts[1]=cmask[i], p):("'\\0'"));
	}
	printf("\n");
}

int main(int argc, char const *argv[])
{
	int i;
	memset(cmask, 0, sizeof(cmask));

	cmask['#'] = '#';
	cmask['\''] = '\'';
	cmask[','] = ',';
	cmask['+'] = '+';
	cmask['-'] = '-';
	cmask['*'] = '*';
	cmask['/'] = '/';
	cmask['>'] = '>';
	cmask['<'] = '<';
	cmask['='] = '=';
	cmask['!'] = '!';
	cmask['('] = '(';
	cmask[')'] = ')';
	cmask['{'] = '{';
	cmask['}'] = '}';
	cmask['&'] = '&';
	cmask['|'] = '|';
	cmask['.'] = '.';
	cmask['\n'] = '\n';
	cmask[' '] = ' ';
	cmask['\t'] = '\t';
	cmask[';'] = ';';
	cmask['['] = '[';
	cmask[']'] = ']';

	for(i='0'; i<='9'; i++){
		cmask[i] = '0';
	}

	for(i='a'; i<='z'; i++){
		cmask[i] = 'A';
	}

	for(i='A'; i<='Z'; i++){
		cmask[i] = 'A';
	}

	cmask['_'] = 'A';

	print();
	return 0;
}