#define linit_c
#define LUA_LIB
#define LUAC_CROSS_FILE

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
#include "modules.h"
#include "esp_misc.h"
#include "lrotable.h"
#include "user_modules.h"

#include "c_string.h"

extern const luaR_entry i2c_map[];
extern const luaR_entry pwm_map[];
extern const luaR_entry gpio_map[];
extern const luaR_entry tmr_map[];
extern const luaR_entry node_map[];
extern const luaR_entry uart_map[];
extern const luaR_entry file_map[];
extern const luaR_entry utils_map[];
extern const luaR_entry mqtt_map[];
extern const luaR_entry strlib[];
extern const luaR_entry math_map[];
extern const luaR_entry tab_funcs[];

static const luaL_Reg lua_libs[] = {
  {"base", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_STRLIBNAME, luaopen_string},    
//  {LUA_MATHLIBNAME, luaopen_math},
//  {LUA_TABLIBNAME, luaopen_table},

#ifdef LUA_PLATFORM_LIBS_REG
  LUA_PLATFORM_LIBS_REG,
#endif 
#if defined(LUA_PLATFORM_LIBS_ROM)
  LUA_PLATFORM_LIBS_ROM
#endif
#if defined(LUA_LIBS_NOLTR)
  LUA_LIBS_NOLTR
#endif
  {NULL, NULL}
};

const luaR_table lua_rotable[] = 
{
    {LUA_STRLIBNAME, strlib},
    {LUA_MATHLIBNAME, math_map},
    {LUA_TABLIBNAME, tab_funcs},
#if LUA_OPTIMIZE_MEMORY > 0
#ifdef USE_FILE_MODULE
	{"file", file_map},
#endif

#ifdef USE_NODE_MODULE
    {"node", node_map},
#endif

#ifdef USE_UART_MODULE
	{"uart", uart_map},
#endif

#ifdef USE_UTILS_MODULE
	{"utils", utils_map},
#endif

#ifdef USE_MQTT_MODULE
	{"mqtt", mqtt_map},
#endif

#ifdef USE_TMR_MODULE
	{"tmr", tmr_map},
#endif

#ifdef USE_GPIO_MODULE
	{"gpio", gpio_map},
#endif

#ifdef USE_PWM_MODULE
	{"pwm", pwm_map},
#endif

#ifdef USE_I2C_MODULE
	{"i2c", i2c_map},
#endif
    
#if defined(LUA_PLATFORM_LIBS_ROM) && LUA_OPTIMIZE_MEMORY == 2
#undef _ROM
#define _ROM( name, openf, table ) { name, table },
  LUA_PLATFORM_LIBS_ROM
#endif
#endif
    
  {NULL, NULL}
};


void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lua_libs;
  for (; lib->name; lib++) {
	os_printf("load lib: %s\n", lib->name);
    if (lib->func)
    {
      lua_pushcfunction(L, lib->func);
      lua_pushstring(L, lib->name);
      lua_call(L, 1, 0);
    }
  }
  os_printf("other libs:\n");

#ifdef USE_FILE_MODULE
	luaopen_file(L);
#endif

#ifdef USE_NODE_MODULE
	luaopen_node(L);
#endif

#ifdef USE_LPEG_MODULE
	luaopen_lpeg(L);
#endif

#ifdef USE_UART_MODULE
	luaopen_uart(L);
#endif

#ifdef USE_UTILS_MODULE
	luaopen_utils(L);
#endif

#ifdef USE_MQTT_MODULE
	luaopen_mqtt(L);
#endif

#ifdef USE_TMR_MODULE
	luaopen_tmr(L);
#endif

#ifdef USE_GPIO_MODULE
	luaopen_gpio(L);
#endif

#ifdef USE_PWM_MODULE
	luaopen_pwm(L);
#endif

#ifdef USE_I2C_MODULE
	luaopen_i2c(L);
#endif
}