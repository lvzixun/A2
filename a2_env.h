#ifndef _A2_ENV_H_
#define _A2_ENV_H_


struct a2_env;

// new / free
struct a2_env* a2_env_new();
void a2_env_free(struct a2_env* env_p);

char* a2_env_addstr(struct a2_env* env_p, char* a2_str);	

#endif