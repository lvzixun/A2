#ifndef _A2_ENV_H_
#define _A2_ENV_H_


struct a2_env;
struct a2_token;

// new / free
struct a2_env* a2_env_new();
void a2_env_free(struct a2_env* env_p);

inline char* a2_env_addstr(struct a2_env* env_p, char* a2_str);	
struct a2_gcobj* a2_env_addstrobj(struct a2_env* env_p, char* a2_s);

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

#endif