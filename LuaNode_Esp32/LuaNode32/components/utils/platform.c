#include "esp_system.h"
#include "esp_log.h"
#include "platform.h"
#include "driver/gpio.h"

#define TAG		"platform"

void lua_node_restart()
{
#if (CURRENT_PLATFORM == NODE_PLATFORM_ESP32)
	esp_restart();
#elif (CURRENT_PLATFORM == NODE_PLATFORM_ESP8266)
	system_restart();
#else
	ESP_LOGE(TAG, "Not define CURRENT_PLATFORM!");
#endif
}

void lua_node_sleep(uint64_t us)
{
#if (CURRENT_PLATFORM == NODE_PLATFORM_ESP32)
	//system_deep_sleep (us);
#else
	ESP_LOGE(TAG, "Not define CURRENT_PLATFORM!");
#endif
}

uint32_t lua_node_get_heap_size(void)
{
	uint32_t heap_size = 0;
#if (CURRENT_PLATFORM == NODE_PLATFORM_ESP32)
	heap_size = esp_get_minimum_free_heap_size();
#else
	ESP_LOGE(TAG, "Not define CURRENT_PLATFORM!");
#endif
	return heap_size;
}

void lua_node_system_restore(void)
{
	
}

//////////////////////////////////////////////////
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_NUM_15))

int platform_gpio_mode(int pin, int mode, int type)
{
	gpio_config_t io_conf = {};
    //disable interrupt 
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; 
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
	
	return ESP_OK;
}

void platform_gpio_isr_uninstall(void)
{
	
}

uint8_t platform_gpio_read(int pin)
{
	
	return 0;
}

void platform_gpio_write(int pin, int level)
{
	
}