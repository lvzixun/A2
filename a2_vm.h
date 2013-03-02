#ifndef _A2_VM_H_
#define _A2_VM_H_
#include "a2_conf.h"

struct a2_env;
struct a2_vm;
struct vm_callinfo;

struct a2_vm* a2_vm_new(struct a2_env* env_p);
void a2_vm_free(struct a2_vm* vm_p);

void a2_vm_load(struct a2_vm* vm_p, struct a2_closure* cls);

inline size_t vm_callinfo_sfi(struct vm_callinfo* ci, size_t reg_idx);
inline struct a2_closure* vm_callinfo_cls(struct vm_callinfo* ci);
inline struct a2_obj* vm_sf_index(struct a2_vm* vm_p, size_t sf_idx);

#endif

