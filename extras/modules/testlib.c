// Module for interfacing with GPIO

#include "modules.h"
#include "lauxlib.h"

int test_add(lua_State *L){
  int a = lua_tointeger(L, 1);
  int b = lua_tointeger(L, 2);
  lua_pushinteger(L, a+b);
  return 1;
}


// Module function map
static const LUA_REG_TYPE testlib_map[] = {
  { LSTRKEY( "test_add" ),   LFUNCVAL( test_add ) },
  { LNILKEY, LNILVAL }
};


NODEMCU_MODULE(TESTLIB, "testlib", testlib_map, NULL);
