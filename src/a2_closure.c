#include "a2_conf.h"
#include "a2_obj.h"
#include "a2_xclosure.h"
#include "a2_closure.h"
#include "a2_gc.h"
#include "a2_env.h"

#define DEF_UD_SIZE	8


static inline void _closure_add_uped(struct a2_closure* cls, struct a2_upvalue* uv_p);

struct a2_closure* a2_closure_new(struct vm_callinfo* ci, int idx){
	struct a2_xclosure* xcls_p = xcls_xcls(vm_callinfo_cls(ci)->xcls_p, idx);
	xcls_add_refs(xcls_p);
	int _s = xcls_p->upvaluex.size;
	struct a2_closure* cls = (struct a2_closure*)malloc(
		sizeof(*cls)+sizeof(struct a2_upvalue)*_s);

	cls->uv_size = _s;
	cls->xcls_p = xcls_p;

	//init uped value stack frame
	cls->ud_sf_gc.size = a2_closure_regscount(cls);
	cls->ud_sf_gc.ud_gc_p = (struct a2_gcobj**)calloc(cls->ud_sf_gc.size, sizeof(struct a2_gcobj*));

	// init uped
	cls->ud_size = DEF_UD_SIZE;
	cls->ud_cap = 0;
	cls->uped_chain = (struct a2_upvalue**)calloc(cls->ud_size, 
		sizeof(struct a2_upvalue*));

	struct a2_closure* _cls = vm_callinfo_cls(ci);
	
	// init upvalue
	int i;
	for(i=0; i<xcls_p->upvaluex.len; i++){
		struct upvaluex_idx* uvx_idx = &(xcls_p->upvaluex.upvaluex_chain[i]);
		switch(uvx_idx->uvx_type){
			case uvx_reg:{
					// set upvalue
					cls->uv_chain[i].type = uv_stack;
					cls->uv_chain[i].cls = _cls;
					cls->uv_chain[i].v.sf_idx = vm_callinfo_sfi(ci, uvx_idx->idx.regs_idx);

					// set uped 
					_closure_add_uped(_cls, &(cls->uv_chain[i]));
				}
				break;
			case uvx_upvalue:{
					assert(uvx_idx->idx.upvalue_idx < _cls->uv_size);
					cls->uv_chain[i] = _cls->uv_chain[uvx_idx->idx.upvalue_idx];
					// set uped
					if(cls->uv_chain[i].type == uv_stack)
						_closure_add_uped(cls->uv_chain[i].cls, &(cls->uv_chain[i]));
				}
				break;
			default:
				assert(0);
		}
	}
	return cls;
}


struct a2_closure* a2_closure_newrun(struct a2_xclosure* xcls){
	assert(xcls);
	struct a2_closure* cls = (struct a2_closure*)malloc(sizeof(*cls));
	xcls_add_refs(xcls);
	cls->xcls_p = xcls;

	cls->ud_sf_gc.ud_gc_p = NULL;
	cls->ud_sf_gc.size = 0;

	// init upvalue
	cls->uv_size = 0;
	cls->uped_chain = NULL;

	// init uped
	cls->ud_size = DEF_UD_SIZE;
	cls->ud_cap = 0;
	cls->uped_chain = (struct a2_upvalue**)calloc(cls->ud_size, 
		sizeof(struct a2_upvalue*));

	return cls;
}

void a2_closure_free(struct a2_closure* cls){
	xcls_del_refs(cls->xcls_p);
	if(cls->uped_chain)
		free(cls->uped_chain);
	if(cls->ud_sf_gc.ud_gc_p)
		free(cls->ud_sf_gc.ud_gc_p);
	free(cls);
}

// the upvalue will set uv_gc when the closure return
void a2_closure_return(struct a2_closure* cls, struct a2_env* env_p){
	assert(cls);
	int i;
	struct vm_callinfo* curr_ci = vm_curr_callinfo(a2_envvm(env_p));

	for(i=0; i<cls->ud_cap; i++){
		assert(cls->ud_sf_gc.ud_gc_p && cls->ud_sf_gc.size>0);
		assert(cls->uped_chain[i]);
		assert(cls->uped_chain[i]->type == uv_stack);

		cls->uped_chain[i]->type = uv_gc;
		cls->uped_chain[i]->cls = NULL;
		
		size_t idx = vm_callinfo_regi(curr_ci, cls->uped_chain[i]->v.sf_idx);
		assert(idx< cls->ud_sf_gc.size);
		struct a2_gcobj** _gcobj = &(cls->ud_sf_gc.ud_gc_p[idx]);

		if(*_gcobj == NULL){
			struct a2_obj* obj = a2_sfidx(env_p, cls->uped_chain[i]->v.sf_idx);
			*_gcobj = a2_upvalue2gcobj(obj);
			a2_gcadd(env_p, (*_gcobj));
		}
		cls->uped_chain[i]->v.uv_obj = *_gcobj;
	}
	cls->ud_cap = 0;
}

inline int a2_closure_params(struct a2_closure* cls){
	assert(cls);
	return cls->xcls_p->params;
}


static inline void _closure_add_uped(struct a2_closure* cls, struct a2_upvalue* uv_p){
	assert(uv_p);
	if(cls->ud_cap>= cls->ud_size){
		cls->ud_size *= 2;
		cls->uped_chain = (struct a2_upvalue**)realloc(cls->uped_chain,
			 cls->ud_size*sizeof(struct a2_upvalue*));
	}

	cls->uped_chain[cls->ud_cap++] = uv_p;
}
