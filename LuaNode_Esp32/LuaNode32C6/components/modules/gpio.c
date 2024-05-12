// Module for interfacing with GPIO

//#include "modules.h"
#include "lauxlib.h"
#include "lualib.h"
//#include "lrotable.h"
#include "platform.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "c_types.h"
#include "c_string.h"


/*#undef MIN_OPT_LEVEL
#define MIN_OPT_LEVEL 1
#define LUA_OPTIMIZE_MEMORY 2*/
#include "lrodefs.h"


#define GPIO_INTERRUPT_ENABLE

#define TAG	"gpio"

#ifdef GPIO_INTERRUPT_ENABLE
static int gpio_cb_ref[GPIO_PIN_NUM];
static lua_State* gL = NULL;

void lua_gpio_unref(unsigned pin){
  if(gpio_cb_ref[pin] != LUA_NOREF){
    if(gL!=NULL)
      luaL_unref(gL, LUA_REGISTRYINDEX, gpio_cb_ref[pin]);
  }
  gpio_cb_ref[pin] = LUA_NOREF;
}

void gpio_intr_callback( unsigned pin, unsigned level )
{
  ESP_LOGI(TAG, "pin:%d, level:%d \n", pin, level);
  if(gpio_cb_ref[pin] == LUA_NOREF)
    return;
  if(!gL)
    return;
  lua_rawgeti(gL, LUA_REGISTRYINDEX, gpio_cb_ref[pin]);
  lua_pushinteger(gL, level);
  lua_call(gL, 1, 0);
}

// Lua: remove( pin )
static int lgpio_remove_isr( lua_State* L )
{
	unsigned pin;
	pin = luaL_checkinteger( L, 1 );
	esp_err_t err = gpio_isr_handler_remove( pin );
	if (err != ESP_OK) {
		lua_pushinteger( L, pin );
		return 1;
	}
	lua_pushinteger( L, ESP_OK );
	return 1;
}

// Lua: uninstall()
static int lgpio_uninstall( lua_State* L )
{
	gpio_uninstall_isr_service();
	platform_gpio_isr_uninstall();
	return 0;
}

#endif

// Lua: mode( pin, mode, type, function )
static int lgpio_mode( lua_State* L )
{
  unsigned mode;
  unsigned pin;
  size_t sl;
  int type = GPIO_INTR_DISABLE;

  pin = luaL_checkinteger( L, 1 );
  mode = luaL_checkinteger( L, 2 );
  if ( mode!=OUTPUT && mode!=INPUT && mode!=PLATFORM_INTERRUPT && mode!= INOUT)
    return luaL_error( L, "wrong arg type" );
  if(pin==0 && mode==PLATFORM_INTERRUPT)
    return luaL_error( L, "no interrupt for D0" );
  if(lua_gettop(L) > 2) {
	const char *str = luaL_checklstring( L, 3, &sl );
	if (str == NULL) {
	  return luaL_error( L, "wrong arg type" );
	}
	if(sl == 2 && strcmp(str, "up") == 0){
		type = GPIO_INTR_POSEDGE; 
	}else if(sl == 4 && strcmp(str, "down") == 0){
		type = GPIO_INTR_NEGEDGE; 
	}else if(sl == 4 && strcmp(str, "both") == 0){
		type = GPIO_INTR_ANYEDGE;
	}else if(sl == 3 && strcmp(str, "low") == 0){
		type = GPIO_INTR_LOW_LEVEL;
	}else if(sl == 4 && strcmp(str, "high") == 0){
		type = GPIO_INTR_HIGH_LEVEL;
	}else{
		type = GPIO_INTR_DISABLE; printf("==> 6\n");
	}
  }
  
  if (lua_gettop(L) > 3) {
	if (lua_type(L, 4) == LUA_TFUNCTION || lua_type(L, 4) == LUA_TLIGHTFUNCTION){
		lua_pushvalue(L, 4);  // copy argument (func) to the top of stack
		if(gpio_cb_ref[pin] != LUA_NOREF)
			luaL_unref(L, LUA_REGISTRYINDEX, gpio_cb_ref[pin]);
		gpio_cb_ref[pin] = luaL_ref(L, LUA_REGISTRYINDEX);
	}
  }
#ifdef GPIO_INTERRUPT_ENABLE
  gL = L;   // save to local gL, for callback function
  if (mode!=PLATFORM_INTERRUPT){     // disable interrupt
    if(gpio_cb_ref[pin] != LUA_NOREF){
      luaL_unref(L, LUA_REGISTRYINDEX, gpio_cb_ref[pin]);
    }
    gpio_cb_ref[pin] = LUA_NOREF;
  }
#endif
  int r = platform_gpio_mode( pin, mode, type ); 
  if( r<0 )
    return luaL_error( L, "wrong pin num." );

  return 0;  
}

// Lua: read( pin )
static int lgpio_read( lua_State* L )
{
  unsigned pin;
  
  pin = luaL_checkinteger( L, 1 );
  //MOD_CHECK_ID( gpio, pin );

  unsigned level = platform_gpio_read( pin );
  lua_pushinteger( L, level );
  return 1; 
}

// Lua: write( pin, level )
static int lgpio_write( lua_State* L )
{
  unsigned level;
  unsigned pin;
  
  pin = luaL_checkinteger( L, 1 );
  //MOD_CHECK_ID( gpio, pin );
  level = luaL_checkinteger( L, 2 );
  if ( level!=HIGH && level!=LOW )
    return luaL_error( L, "wrong arg type" );
  platform_gpio_write(pin, level);
  return 0;  
}

#define DELAY_TABLE_MAX_LEN 256
#define noInterrupts ets_intr_lock
#define interrupts ets_intr_unlock

// Lua: serout( pin, firstLevel, delay_table, [repeatNum] )
// -- serout( pin, firstLevel, delay_table, [repeatNum] )
// gpio.mode(1,gpio.OUTPUT,gpio.PULLUP)
// gpio.serout(1,1,{30,30,60,60,30,30})  -- serial one byte, b10110010
// gpio.serout(1,1,{30,70},8)  -- serial 30% pwm 10k, lasts 8 cycles
// gpio.serout(1,1,{3,7},8)  -- serial 30% pwm 100k, lasts 8 cycles
// gpio.serout(1,1,{0,0},8)  -- serial 50% pwm as fast as possible, lasts 8 cycles
// gpio.mode(1,gpio.OUTPUT,gpio.PULLUP)
// gpio.serout(1,0,{20,10,10,20,10,10,10,100}) -- sim uart one byte 0x5A at about 100kbps
// gpio.serout(1,1,{8,18},8) -- serial 30% pwm 38k, lasts 8 cycles
static int lgpio_serout( lua_State* L )
{
  unsigned level;
  unsigned pin;
  unsigned table_len = 0;
  int repeat = 0;
  int delay_table[DELAY_TABLE_MAX_LEN];
  
  pin = luaL_checkinteger( L, 1 );
  //MOD_CHECK_ID( gpio, pin );
  level = luaL_checkinteger( L, 2 );
  if ( level!=HIGH && level!=LOW )
    return luaL_error( L, "wrong arg type" );
  if( lua_istable( L, 3 ) )
  {
    table_len = lua_objlen( L, 3 );
    if (table_len <= 0 || table_len>DELAY_TABLE_MAX_LEN)
      return luaL_error( L, "wrong arg range" );
    int i;
    for( i = 0; i < table_len; i ++ )
    {
      lua_rawgeti( L, 3, i + 1 );
      delay_table[i] = ( int )luaL_checkinteger( L, -1 );
      lua_pop( L, 1 );
      if( delay_table[i] < 0 || delay_table[i] > 1000000 )    // can not delay more than 1000000 us
        return luaL_error( L, "delay must < 1000000 us" );
    }
  } else {
    return luaL_error( L, "wrong arg range" );
  } 

  if(lua_isnumber(L, 4))
    repeat = lua_tointeger( L, 4 );
  if( repeat < 0 || repeat > DELAY_TABLE_MAX_LEN )
    return luaL_error( L, "delay must < 256" );

  if(repeat==0)
    repeat = 1;
  int j;
  bool skip_loop = true;
  do
  {
    if(skip_loop){    // skip the first loop.
      skip_loop = false;
      continue;
    }
    for(j=0;j<table_len;j++){
      //noInterrupts();
      // platform_gpio_write(pin, level);
      platform_gpio_write(pin, level);
      //interrupts();
	  vTaskDelay(delay_table[j] / portTICK_PERIOD_MS);
      level=!level;
    }
    repeat--;
  } while (repeat>0); 

  return 0;  
}
#undef DELAY_TABLE_MAX_LEN


// Module function map 
const LUA_REG_TYPE gpio_map[] = {
  { LSTRKEY( "mode" ),   LFUNCVAL( lgpio_mode ) },
  { LSTRKEY( "read" ),   LFUNCVAL( lgpio_read ) },
  { LSTRKEY( "write" ),  LFUNCVAL( lgpio_write ) },
  { LSTRKEY( "serout" ), LFUNCVAL( lgpio_serout ) },
#ifdef GPIO_INTERRUPT_ENABLE
  { LSTRKEY( "remove" ),   LFUNCVAL( lgpio_remove_isr ) },
  { LSTRKEY( "uninstall" ),   LFUNCVAL( lgpio_uninstall ) },
  //{ LSTRKEY( "INT" ),    LNUMVAL( PLATFORM_INTERRUPT ) },
#endif
/*  { LSTRKEY( "OUTPUT" ), LNUMVAL( OUTPUT ) },
  { LSTRKEY( "INPUT" ),  LNUMVAL( INPUT ) },
  { LSTRKEY( "INOUT" ),  LNUMVAL( INOUT ) },
  { LSTRKEY( "HIGH" ),   LNUMVAL( HIGH ) },
  { LSTRKEY( "LOW" ),    LNUMVAL( LOW ) },
  { LSTRKEY( "FLOAT" ),  LNUMVAL( FLOAT ) },
  { LSTRKEY( "PULLUP" ), LNUMVAL( PULLUP ) },*/
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_gpio(lua_State *L)
{
	luaL_register( L, LUA_GPIOLIBNAME, gpio_map );
	return 1;
}