#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int lua_ms_time(struct lua_State* L){
  struct timeval st;
  gettimeofday(&st, 0);

  lua_pushnumber(L, (st.tv_sec*1000000+st.tv_usec)/1000000.0);
  return 1;
}

int luaopen_m_time(lua_State* L){
  luaL_checkversion(L);
  luaL_Reg l[] = {
    {"ms_time", lua_ms_time},
    {NULL, NULL},
  };

  luaL_newlib(L, l);
  return 1;
}

