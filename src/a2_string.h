#ifndef _A2_STRING_H_
#define _A2_STRING_H_
#include "a2_conf.h"

#define a2_str_new a2_string_new("\0")

char* a2_string_new(const char* s);
void a2_string_free(char* a2_s);
void a2_string_clear(char* a2_s);

char* a2_string_append(char* a2_s, char c);
inline size_t a2_string_len(char* a2_s);
inline char* a2_string_cat(char* a2_s, char* str);

#endif
