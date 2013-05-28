#ifndef _A2_VM_H_
#define _A2_VM_H_
#include "a2_conf.h"

struct a2_env;
struct a2_vm;
struct vm_callinfo;

struct a2_vm* a2_vm_new(struct a2_env* env_p);
void a2_vm_free(struct a2_vm* vm_p);

void a2_vm_gc(struct a2_vm* vm_p);

int a2_vm_load(struct a2_vm* vm_p, struct a2_closure* cls);

int a2_vm_pcall(struct a2_vm* vm_p, struct a2_obj* cls_obj, struct a2_obj* args_obj, int args);

inline size_t vm_callinfo_sfi(struct vm_callinfo* ci, size_t reg_idx);
inline size_t vm_callinfo_regi(struct vm_callinfo* ci, size_t sf_idx);
inline  struct vm_callinfo* vm_curr_callinfo(struct a2_vm* vm_p);
inline struct a2_closure* vm_callinfo_cls(struct vm_callinfo* ci);
inline struct a2_obj* vm_sf_index(struct a2_vm* vm_p, size_t sf_idx);

#endif

