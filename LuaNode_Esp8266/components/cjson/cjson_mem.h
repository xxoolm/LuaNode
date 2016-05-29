#ifndef _CJSON_MEM_H_
#define _CJSON_MEM_H_

#include "lua.h"

void cjson_mem_setlua (lua_State *L);

#define cjson_mem_malloc	malloc;
#define cjson_mem_realloc	realloc;

#endif
