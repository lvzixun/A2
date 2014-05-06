#ifndef _A2_ENV_H_
#define _A2_ENV_H_
#include "a2_conf.h"

enum mk{
  MK_INDEX,
  // no support
  MK_NEWINDEX,
  MK_GC,
  MK_CALL,

  MK_COUNT
};

struct a2_state;
struct a2_io;
struct a2_env;
struct a2_token;
struct a2_obj;

// new / free
struct a2_env* a2_env_new(struct a2_state* state);
void a2_env_free(struct a2_env* env_p);

// load
int a2_env_load(struct a2_env* env_p, struct a2_io* stream);
inline struct a2_state* a2_env2state(struct a2_env* env_p);

// c <-> a2 stack
inline void a2_setcstack(struct a2_env* env_p, int idx, struct a2_obj* obj);
inline struct a2_obj* a2_getcstk_top(struct a2_env* env_p);
inline void a2_pushstack(struct a2_env* env_p, struct a2_obj* v);
inline void a2_setbottom(struct a2_env* env_p, int bottom);
inline int a2_getbottom(struct a2_env* env_p);
inline int a2_gettop(struct a2_env* env_p);
inline void a2_settop(struct a2_env* env_p, int top);
inline struct a2_obj* a2_getcstack(struct a2_env* env_p, int idx);

// global map 
inline struct a2_obj* a2_get_envglobal(struct a2_env* env_p, struct a2_obj* k);
inline struct a2_obj* a2_set_envglobal(struct a2_env* env_p, struct a2_obj* k, struct a2_obj* v);

// register map
inline struct a2_obj* a2_get_envreg(struct a2_env* env_p, struct a2_obj* k);
inline struct a2_obj* a2_set_envreg(struct a2_env* env_p, struct a2_obj* k, struct a2_obj* v);

// meta key
struct a2_obj* a2_env_getmk(struct a2_env* env_p, enum mk t);

// global string
inline struct a2_gcobj* a2_env_addstrobj(struct a2_env* env_p, char* a2_s);
inline struct a2_obj a2_env_addstr(struct a2_env* env_p, char* str);
void a2_env_clear_itstring(struct a2_env* env_p, struct a2_gcobj* str_gcobj);

//  gc it
inline void a2_vmgc(struct a2_env* env_p);

// ir operation function
inline void a2_irexec(struct a2_env* env_p, size_t root);
inline struct a2_xclosure* a2_irexend(struct a2_env* env_p);
// parse operation function
inline struct a2_node* a2_nodep(struct a2_env* env_p, size_t idx);
// add gc object
inline  void a2_gcadd(struct a2_env* env_p, struct a2_gcobj* gcobj);
// the obj from stack frame at sf_idx
inline struct a2_obj* a2_sfidx(struct a2_env* env_p, size_t sf_idx);


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


inline struct a2_lex* a2_envlex(struct a2_env* env_p);
inline struct a2_parse* a2_envparse(struct a2_env* env_p);
inline struct a2_ir* a2_envir(struct a2_env* env_p);
inline struct a2_gc* a2_envgc(struct a2_env* env_p);
inline struct a2_longjump** a2_envjp(struct a2_env* env_p);
inline struct a2_vm* a2_envvm(struct a2_env* env_p);


typedef void(*panic_func)(struct a2_state*);
inline panic_func a2_envpanic(struct a2_env* env_p);
inline void a2_setpanic(struct a2_env* env_p, panic_func panic);
inline int a2_callpanic(struct a2_env* env_p);

#endif

