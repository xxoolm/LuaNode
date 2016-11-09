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

extern const LUA_REG_TYPE gpio_map[];
extern const LUA_REG_TYPE pwm_map[];
extern const LUA_REG_TYPE node_map[];
extern const LUA_REG_TYPE wifi_map[];
extern const LUA_REG_TYPE file_map[];
extern const LUA_REG_TYPE tmr_map[];
extern const LUA_REG_TYPE i2c_map[];
extern const LUA_REG_TYPE uart_map[];
extern const LUA_REG_TYPE utils_map[];
extern const LUA_REG_TYPE strlib[];
extern const LUA_REG_TYPE tab_funcs[];

/*BUILTIN_LIB_INIT(BASE, "base", luaopen_base);
BUILTIN_LIB_INIT(LOADLIB, "package", luaopen_package);
BUILTIN_LIB_INIT(STRING, "string", luaopen_string);
BUILTIN_LIB(STRING, "string", strlib);*/

const luaL_Reg lua_libs[] = {
	{"base", luaopen_base},
	{"package", luaopen_package},
#ifdef USE_GPIO_MODULE
	{LUA_GPIOLIBNAME, luaopen_gpio},
#endif
#ifdef USE_PWM_MODULE
	{LUA_PWMLIBNAME, luaopen_pwm},
#endif
#ifdef USE_NODE_MODULE
	{LUA_NODELIBNAME, luaopen_node},
#endif
#ifdef USE_WIFI_MODULE
	{LUA_WIFILIBNAME, luaopen_wifi},
#endif
#ifdef USE_FILE_MODULE
	{LUA_FILELIBNAME, luaopen_file},
#endif
#ifdef USE_TMR_MODULE
	{LUA_TMRLIBNAME, luaopen_tmr},
#endif
#ifdef USE_I2C_MODULE
	{LUA_I2CLIBNAME, luaopen_i2c},
#endif
#ifdef USE_UART_MODULE
	{LUA_UARTLIBNAME, luaopen_uart},
#endif
#ifdef USE_UTILS_MODULE
	{LUA_UTILSLIBNAME, luaopen_utils},
#endif
	{NULL, NULL},
};

const luaR_table lua_rotable[] = 
{
	{LUA_TABLIBNAME, tab_funcs},
    {LUA_STRLIBNAME, strlib},
#ifdef USE_GPIO_MODULE
	{LUA_GPIOLIBNAME, gpio_map},
#endif
#ifdef USE_PWM_MODULE
	{LUA_PWMLIBNAME, pwm_map},
#endif
#ifdef USE_NODE_MODULE
	{LUA_NODELIBNAME, node_map},
#endif
#ifdef USE_WIFI_MODULE
	{LUA_WIFILIBNAME, wifi_map},
#endif
#ifdef USE_FILE_MODULE
	{LUA_FILELIBNAME, file_map},
#endif
#ifdef USE_TMR_MODULE
	{LUA_TMRLIBNAME, tmr_map},
#endif
#ifdef USE_I2C_MODULE
	{LUA_I2CLIBNAME, i2c_map},
#endif
#ifdef USE_UART_MODULE
	{LUA_UARTLIBNAME, uart_map},
#endif
#ifdef USE_UTILS_MODULE
	{LUA_UTILSLIBNAME, utils_map},
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

	//luaopen_pwm(L);
}