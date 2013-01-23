#ifndef  _A2_H_
#define  _A2_H_
#include "a2_conf.h"

struct a2_state;

typedef int (*a2_cfunction)(struct a2_state*);
A2_API struct a2_state* a2_open();
A2_API void a2_close(struct a2_state* state);	


#endif
