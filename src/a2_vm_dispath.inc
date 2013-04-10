/*
 the operate of ir  execute. 
*/

dispath_op(NIL){
	assert(0);
	printf("nil\n");
}

dispath_op(GETGLOBAL){
	int bx = ir_gbx(curr_ir);
	struct a2_obj* _dobj = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, bx);
	struct a2_obj* _obj = a2_get_envglobal(vm_p->env_p, _k);

	if(_obj==NULL){
		char _buf[64] = {0};
		a2_error(vm_p->env_p, e_vm_error, 
			"[vm error@%zd]: the global \'%s\' is not find.\n", 
			curr_line, 
			obj2str(_k, _buf, sizeof(_buf)));
	}
	*_dobj = *_obj;
	curr_pc++;
	dispath_end;
}

dispath_op(SETGLOBAL){
	struct a2_obj* _k = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	
	*_d = *a2_set_envglobal(vm_p->env_p, _k, _v);
	curr_pc++;
	dispath_end;	
}

dispath_op(GETUPVALUE){
	struct a2_upvalue* _uv = a2_closure_upvalue(curr_cls, ir_gbx(curr_ir));
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));

	switch(_uv->type){
		case uv_stack:
			*_d = *sf_reg(_uv->v.sf_idx);
			break;
		case uv_gc:
			*_d = *a2_gcobj2upvalue(_uv->v.uv_obj);
			break;
		default:
			assert(0);
	}
	curr_pc++;
	dispath_end;
}

dispath_op(SETUPVALUE){
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));
	struct a2_upvalue* _uv_d = a2_closure_upvalue(curr_cls, ir_ga(curr_ir));
	struct a2_obj* _ud = NULL;

	switch(_uv_d->type){
		case uv_stack:
			*sf_reg(_uv_d->v.sf_idx) = *_v;
			break;
		case uv_gc:
			_ud = a2_gcobj2upvalue(_uv_d->v.uv_obj);
			*_ud = *_v;
			break;
		default:
			assert(0);
	}
	curr_pc++;
	dispath_end;
}

dispath_op(NEWLIST){
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	struct a2_gcobj* _gcobj = a2_array2gcobj(a2_array_new());
	a2_gcadd(vm_p->env_p, _gcobj);
	obj_setX(_d, A2_TARRAY, obj,  _gcobj);
	curr_pc++;
	dispath_end;
}

dispath_op(NEWMAP){
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	struct a2_gcobj* _gcobj = a2_map2gcobj(a2_map_new());
	a2_gcadd(vm_p->env_p, _gcobj);
	obj_setX(_d, A2_TMAP, obj, _gcobj);
	curr_pc++;
	dispath_end;
}

dispath_op(SETLIST){
	int i, end=ir_gb(curr_ir)+ir_gc(curr_ir);
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));	
	assert(obj_t(_d)==A2_TARRAY);
	for(i=ir_gb(curr_ir); i<end; i++){
		a2_array_add(a2_gcobj2array(obj_vX(_d, obj)), callinfo_sfreg(curr_ci, i));
	}
	curr_pc++;
	dispath_end;
}

dispath_op(SETMAP){
	int i, end=ir_gb(curr_ir)+2*ir_gc(curr_ir);
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = NULL;
	struct a2_map* map = NULL;
	assert(obj_t(_d)==A2_TMAP);
	
	struct a2_kv kv={0};
	for(i=ir_gb(curr_ir); i<end; i+=2){
		kv.key = callinfo_sfreg(curr_ci, i);
		kv.vp = callinfo_sfreg(curr_ci, i+1);
		map = a2_gcobj2map(obj_vX(_d, obj));
		if( (_v=a2_map_query(map, kv.key))==NULL )
			a2_map_add(map, &kv);
		else
			*_v = *kv.vp;
	}
	curr_pc++;
	dispath_end;
}

dispath_op(GETVALUE){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_gb(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(obj_t(_c)){
		case A2_TARRAY:
	 		if(obj_t(_k)!=A2_TNUMBER)
	 			vm_error("the key is must number at get array.");
			__d = a2_array_get(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(!__d) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		case A2_TMAP:
			if(obj_t(_k)!=A2_TNUMBER && obj_t(_k)!=A2_TSTRING)
				vm_error("the key is must number or string at get map.");
			__d = a2_map_query(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(!__d) goto GVALUE_ERROR;
			*_d = *__d;
			break;
		default:
			vm_error("the varable is not map or array.");
	}

 	curr_pc++;
 	dispath_end;
GVALUE_ERROR:
 	vm_error("the key is overfllow.");
}

dispath_op(SETVALUE){
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _k = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gc(curr_ir));
	struct a2_obj* __d = NULL;
	switch(obj_t(_c)){
		case A2_TARRAY:
	 		if(obj_t(_k)!=A2_TNUMBER)
	 			vm_error("the key is must number at set array.");
			__d = a2_array_get(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(!__d)  goto SVALUE_ERROR;
			*__d = *_v;
			break;
		case A2_TMAP:
			if(obj_t(_k)!=A2_TNUMBER && obj_t(_k)!=A2_TSTRING)
				vm_error("the key is must number or string at set map.");
			__d = a2_map_query(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(!__d) goto SVALUE_ERROR;
			*__d = *_v;
			break;
		default:
			vm_error("the varable is not map or array.");
	}

 	curr_pc++;
 	dispath_end;
 SVALUE_ERROR:
 	vm_error("the key is overfllow.");
}

dispath_op(CLOSURE){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_closure* _cls = a2_closure_new(curr_ci, ir_gbx(curr_ir));
	struct a2_gcobj* _gcobj = a2_closure2gcobj(_cls);
	obj_setX(_d, A2_TCLOSURE, obj, _gcobj);
	a2_gcadd(vm_p->env_p, _gcobj);
	curr_pc++;
	dispath_end;	
}

dispath_op(CALL){
	struct a2_obj* _func = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	switch(obj_t(_func)){
		case A2_TCLOSURE:
			__vm_call_function(vm_p, _func);
			break;
		case A2_TCFUNCTION:
			__vm_call_cfunction(vm_p, _func);
			break;
		default:
			vm_error("the varable is not function type.");
	}
	dispath_end;	
}

dispath_op(RETURN){
	if(_vm_return(vm_p, &ret))
					return ret;
	dispath_end;
}

dispath_op(FORPREP){
	struct a2_obj* _i = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _count = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _step = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);

	if(obj_t(_i)!=A2_TNUMBER)
		vm_error("the index varable is must number.");
	if(obj_t(_count)!=A2_TNUMBER)
		vm_error("the for's count is must number.");
	if(obj_t(_step)!=A2_TNUMBER)
		vm_error("the for's step is must number.");

	if(obj_vNum(_i)>=obj_vNum(_count))  // for end
		jump(ir_gbx(curr_ir));
	else	// for continue
		curr_pc++;
	dispath_end;
}

dispath_op(FORLOOP){
	struct a2_obj* _i = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _count = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _step = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);

	if(obj_t(_i)!=A2_TNUMBER)
		vm_error("the index varable is must number.");

	assert(obj_t(_i)==A2_TNUMBER && obj_t(_count)==A2_TNUMBER && obj_t(_step)==A2_TNUMBER);
	obj_vNum(_i) += obj_vNum(_step);
	if(obj_vNum(_i) < obj_vNum(_count)) // continue for
		jump(ir_gbx(curr_ir));
	else // for end
		curr_pc++;
	dispath_end;
}

dispath_op(FOREACHPREP){
	struct a2_obj* _k = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(obj_t(_c)!=A2_TMAP && obj_t(_c)!=A2_TARRAY)
		vm_error("the varable is not map or array.");

	// set init varable
	switch(obj_t(_c)){
		case A2_TMAP:
			*_k = a2_nil2obj();
			__v = a2_map_next(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(__v==NULL)
				jump(ir_gbx(curr_ir));
			else{
				*_v = *__v;
				curr_pc++;
			}
			break;
		case A2_TARRAY:
			*_k = a2_nil2obj();
			__v = a2_array_next(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(__v==NULL)  // dump is end
				jump(ir_gbx(curr_ir));
			else{
				*_v = *__v;
				curr_pc++;
			}
			break;
		default:
			assert(0);
	}
	dispath_end;
}

dispath_op(FOREACHLOOP){
	struct a2_obj* _k = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+1);
	struct a2_obj* _c = callinfo_sfreg(curr_ci, ir_ga(curr_ir)+2);
	struct a2_obj* __v = NULL;

	if(obj_t(_c)!=A2_TMAP && obj_t(_c)!=A2_TARRAY)
		vm_error("the varable is not map or array.");

	// dump next
	switch(obj_t(_c)){
		case A2_TMAP:
			__v = a2_map_next(a2_gcobj2map(obj_vX(_c, obj)), _k);
			if(__v==NULL)
				curr_pc++;
			else{
				*_v = *__v;
				jump(ir_gbx(curr_ir));
			}
			break;
		case A2_TARRAY:
			__v = a2_array_next(a2_gcobj2array(obj_vX(_c, obj)), _k);
			if(__v==NULL)  // dump is end
				curr_pc++;
			else{
				*_v = *__v;
				jump(ir_gbx(curr_ir));
			}
			break;
		default:
			assert(0);
	}
	dispath_end;
}

dispath_op(JUMP){
	jump(ir_gbx(curr_ir));
	dispath_end;
}

dispath_op(MOVE){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	*_d = *callinfo_sfreg(curr_ci, ir_gbx(curr_ir));
	curr_pc++;
	dispath_end;	
}

dispath_op(TEST){
	struct  a2_obj* _v = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	if(obj_t(_v)==A2_TNIL || (obj_t(_v)==A2_TBOOL && !(obj_vX(_v, uinteger))))
		jump(ir_gbx(curr_ir));
	else
		curr_pc++;
	dispath_end;
}

dispath_op(LOAD){
	struct a2_obj* _des_obj = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	*_des_obj = *(a2_closure_const(curr_cls, ir_gbx(curr_ir)));
	curr_pc++;
	dispath_end;
}

dispath_op(LOADNIL){
	int i;
	struct a2_obj* _obj = NULL;
	for(i=ir_ga(curr_ir); i<(ir_ga(curr_ir)+ir_gbx(curr_ir)); i++){
		_obj = callinfo_sfreg(curr_ci, i);
		obj_setX(_obj, A2_TNIL, point, NULL);
	}
	curr_pc++;
	dispath_end;
}

dispath_op(CAT){
	struct a2_obj* _d = _getvalue(vm_p, ir_ga(curr_ir));
	struct a2_obj* _lv = _getvalue(vm_p, ir_gb(curr_ir));
	struct a2_obj* _rv = _getvalue(vm_p, ir_gc(curr_ir));

	char buf[64] = {0};
	char buf0[64] = {0};
	char* a2_s = a2_string_new(obj2str(_lv, buf, sizeof(buf)-1));
	a2_s = a2_string_cat(a2_s, obj2str(_rv, buf0, sizeof(buf0)-1));

	obj_setX(_d, A2_TSTRING, obj, a2_env_addstrobj(vm_p->env_p, a2_s));

	a2_string_free(a2_s);
	curr_pc++;
	dispath_end;
}

dispath_op(ADD){
	_vm_op(+);
	dispath_end;	
}

dispath_op(SUB){
	_vm_op(-);
	dispath_end;
}

dispath_op(MUL){
	_vm_op(*);
	dispath_end;
}

dispath_op(DIV){
	_vm_op(/);
	dispath_end;
}

dispath_op(OR){
	_vm_opl(||);
	dispath_end;
}

dispath_op(AND){
	_vm_opl(&&);
	dispath_end;
}

dispath_op(BIG){
	_vm_oplimit(>);
	dispath_end;
}

dispath_op(LITE){
	_vm_oplimit(<);
	dispath_end;
}

dispath_op(BIGE){
	_vm_oplimit(>=);
	dispath_end;
}

dispath_op(LITEE){
	_vm_oplimit(<=);
	dispath_end;
}

dispath_op(EQU){
	_vm_ope(==);
	dispath_end;
}

dispath_op(NEQU){
	_vm_ope(!=);
	dispath_end;
}

dispath_op(NOT){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	switch(obj_t(_v)){
		case A2_TNIL:
			*_d = a2_bool2obj(1);
			break;
		case A2_TBOOL:
			*_d = a2_bool2obj(!(obj_vX(_v, uinteger)));
			break;
		default:
			vm_error("the varable is not bool type.");
	}

	curr_pc++;
	dispath_end;
}

dispath_op(NEG){
	struct a2_obj* _d = callinfo_sfreg(curr_ci, ir_ga(curr_ir));
	struct a2_obj* _v = _getvalue(vm_p, ir_gbx(curr_ir));

	if(obj_t(_v)!=A2_TNUMBER)
		vm_error("the varable is not number type at set neg.");
	*_d = a2_number2obj(obj_vNum(_v)*(-1));
	curr_pc++;
	dispath_end;
}


