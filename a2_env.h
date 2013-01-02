#ifndef _A2_ENV_H_
#define _A2_ENV_H_
#include "a2_conf.h"

struct a2_env;
struct a2_token;

// new / free
struct a2_env* a2_env_new();
void a2_env_free(struct a2_env* env_p);

struct a2_gcobj* a2_env_addstrobj(struct a2_env* env_p, char* a2_s);

// ir operation function
inline void a2_irexec(struct a2_env* env_p, size_t root);
// parse operation function
inline struct a2_node* a2_nodep(struct a2_env* env_p, size_t idx);

// key token check
inline int a2_ktisfunction(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisreturn(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktiscontinue(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisfor(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisif(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktiselse(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisforeach(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisbreak(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisnil(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktisin(struct a2_env* env_p, struct a2_token* token);
inline int a2_ktislocal(struct a2_env* env_p, struct a2_token* token);

// for test
inline struct a2_lex* a2_envlex(struct a2_env* env_p);
inline struct a2_parse* a2_envparse(struct a2_env* env_p);

#endif