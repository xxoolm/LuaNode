#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define FLASH_AUTOSIZE
#define FLASH_SAFE_API

#define BUILD_SPIFFS	1

#define NODE_ERROR

#ifdef NODE_DEBUG
#define NODE_DBG c_printf
#else
#define NODE_DBG
#endif	/* NODE_DEBUG */

#ifdef NODE_ERROR
#define NODE_ERR c_printf
#else
#define NODE_ERR
#endif	/* NODE_ERROR */

#define ICACHE_STORE_TYPEDEF_ATTR __attribute__((aligned(4),packed))
#define ICACHE_STORE_ATTR __attribute__((aligned(4)))
#define ICACHE_RAM_ATTR __attribute__((section(".iram0.text")))

#define LUA_OPTRAM
#ifdef LUA_OPTRAM
#define LUA_OPTIMIZE_MEMORY			2
#else
#define LUA_OPTIMIZE_MEMORY         0
#endif	/* LUA_OPTRAM */

#define ESP_PLATFORM        1
#define LEWEI_PLATFORM      0

#define USE_OPTIMIZE_PRINTF

#if ESP_PLATFORM
#define PLUG_DEVICE             0
#define LIGHT_DEVICE            1
#define SENSOR_DEVICE			0

#if SENSOR_DEVICE
#define HUMITURE_SUB_DEVICE         1
#define FLAMMABLE_GAS_SUB_DEVICE    0
#endif

//#define SERVER_SSL_ENABLE
//#define CLIENT_SSL_ENABLE
//#define UPGRADE_SSL_ENABLE

#define USE_DNS

#ifdef USE_DNS
#define ESP_DOMAIN      "iot.espressif.cn"
#endif

//#define SOFTAP_ENCRYPT

#ifdef SOFTAP_ENCRYPT
#define PASSWORD	"v*%W>L<@i&Nxe!"
#endif

#if SENSOR_DEVICE
#define SENSOR_DEEP_SLEEP

#if HUMITURE_SUB_DEVICE
#define SENSOR_DEEP_SLEEP_TIME    30000000
#elif FLAMMABLE_GAS_SUB_DEVICE
#define SENSOR_DEEP_SLEEP_TIME    60000000
#endif
#endif

#if LIGHT_DEVICE
#define USE_US_TIMER
#endif

#if PLUG_DEVICE || LIGHT_DEVICE
#define BEACON_TIMEOUT  150000000
#define BEACON_TIME     50000
#endif

#define AP_CACHE           1

#if AP_CACHE
#define AP_CACHE_NUMBER    5
#endif

#elif LEWEI_PLATFORM
#endif

#endif

