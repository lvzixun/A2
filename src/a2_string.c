#include "a2_conf.h"
#include <string.h>
#include <stdio.h>

#define DEFAULT_STRING_DEEP	5
#define DEFAULT_STRING_LEN  (1<<DEFAULT_STRING_DEEP)
#define str_prefix	 ((size_t)(&(((struct a2_string*)0)->str[1])))
#define str_head(s)	 ((struct a2_string*)((void*)(s)-str_prefix))
#define str_level(s) ((byte)(str_head(s)->str[0]))
#define str_deep(s)	 (((size_t)1)<<str_level(s)) 
#define _str(sp)	 ((char*)((void*)(sp)+str_prefix))


struct a2_string{
	size_t len;
	char str[2];
};

static struct a2_string* _resize_string(struct a2_string* a2_s);

inline size_t a2_string_len(char* a2_s){
	check_null(a2_s, 0);
	return str_head(a2_s)->len;
}

char* a2_string_append(char* a2_s, char c){
	size_t len =0;
	check_null(a2_s, 0);
	if(!c)	return a2_s;
	if(a2_string_len(a2_s)>=str_deep(a2_s)){
		a2_s = _str(_resize_string(str_head(a2_s)));
	}
	len = a2_string_len(a2_s);
	a2_s[len]=c;
	a2_s[len+1]='\0';
	str_head(a2_s)->len = len+1;
	return a2_s;
}

inline char* a2_string_cat(char* a2_s, char* str){
	assert(str && a2_s);
	for( ;*str; ){
		a2_s = a2_string_append(a2_s, *str);
		str++;
	}
	return a2_s;
}

char* a2_string_new(const char* s){
	int i=0;
	struct a2_string* ret = NULL;
	check_null(s, NULL);
	ret = _resize_string(NULL);
	while(s[i]){
		ret = str_head(a2_string_append(_str(ret), s[i]));
		i++;
	}
	return _str(ret);
}

static struct a2_string* _resize_string(struct a2_string* a2_p){
	struct a2_string* ret = NULL;
	if(a2_p == NULL){
		ret = (struct a2_string*)malloc(sizeof(*ret)+DEFAULT_STRING_LEN);
		ret->len=0;
		ret->str[0] = DEFAULT_STRING_DEEP;
		_str(ret)[0] = '\0';
	}else{
		a2_p->str[0]++;
		ret = (struct a2_string*)realloc(a2_p, sizeof(*ret)+(1<<a2_p->str[0] ));
	}
	return ret;
}

void a2_string_free(char* a2_s){
	if(a2_s==NULL)
		return;
	free(str_head(a2_s));
}

void a2_string_clear(char* a2_s){
	if(a2_s==NULL)
		return;
	str_head(a2_s)->len = 0;
	a2_s[0] = '\0';
}








