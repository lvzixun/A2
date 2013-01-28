#ifndef _A2_VM_H_
#define _A2_VM_H_

struct a2_vm;

struct a2_vm* a2_vm_new(struct a2_env* env_p);
void a2_vm_free(struct a2_vm* vm_p);

void a2_vm_load(struct a2_vm* vm_p, struct a2_closure* cls);
#endif

