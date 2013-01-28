#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>


int lua_ttime(struct lua_State* L){
	struct timeval st;
 	gettimeofday(&st, 0);

 	lua_pushnumber(L, st.tv_sec*1000000+st.tv_usec);
 	return 1;
}


int main(int argc, char const *argv[])
{	
	struct lua_State* L = luaL_newstate();
	lua_register(L, "t_time", lua_ttime);
	luaL_openlibs(L);
	
	if(luaL_dofile(L, "t.lua")){
		printf("[error]: %s\n", lua_tostring(L, -1));
	}

	lua_close(L);
	return 0;
}
