#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#define NODE_PLATFORM_ESP32		1001
#define NODE_PLATFORM_ESP8266	1002
#define NODE_PLATFORM_STM32		1003
#define NODE_PLATFORM_W800		1004


#define CURRENT_PLATFORM		NODE_PLATFORM_ESP32
#define BUILD_SPIFFS


void lua_node_restart(void);
void lua_node_sleep(uint64_t us);
uint32_t lua_node_get_heap_size(void);
void lua_node_system_restore(void);

int platform_gpio_mode(int pin, int mode, int type);
void platform_gpio_isr_uninstall(void);
uint8_t platform_gpio_read(int pin); 
void platform_gpio_write(int pin, int level);



#endif
