// Module function map
#include "modules.h"
#include "lrodefs.h"
#include "handler.h"

static const LUA_REG_TYPE cjson_map[] = 
{
  { LSTRKEY( "encode" ), LFUNCVAL( json_encode ) },
  { LSTRKEY( "decode" ), LFUNCVAL( json_decode ) },
  { LNILKEY, LNILVAL }
};


LUANODE_MODULE(CJSON, "cjson", cjson_map, NULL);