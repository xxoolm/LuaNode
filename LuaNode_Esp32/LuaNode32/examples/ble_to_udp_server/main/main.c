/**
 * ESP32 beacon scan and udp send demo
 *
 * Nicholas3388
 * 2017.4.12
 */

#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "utils.h"
#include "ble.h"
#include "udp_task.h"
#include "user_config.h"
#include "my_uart.h"
#include "my_list.h"

static const char *TAG = "main";
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static tcpip_adapter_ip_info_t ip_info;

unsigned char wifi_ssid[WIFI_SSID_MAX_LEN] = {0};
unsigned char wifi_pass[WIFI_PASS_MAX_LEN] = {0};
unsigned char srv_ip[SRV_IP_MAX_LEN] = {0};
int srv_port = 0;

extern void connect_server(const char *ip, int port);

static void show_copyright(void)
{
	ESP_LOGI(TAG, "################################");
	ESP_LOGI(TAG, "##         Test app           ##");
	ESP_LOGI(TAG, "##         2017.4.12          ##");
	ESP_LOGI(TAG, "################################");
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void initialise_wifi(void)
{
	if (strlen((const char *)wifi_ssid) == 0) {
		ESP_LOGE(TAG, "expected ssid");
	}
	
	if (strlen((const char *)wifi_pass) == 0) {
		ESP_LOGE(TAG, "expected password");
	}

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
		.sta = {
			.ssid = {0},
			.password = {0},
		},
	};
	memcpy(wifi_config.sta.ssid, wifi_ssid, strlen((const char *)wifi_ssid));
	memcpy(wifi_config.sta.password, wifi_pass, strlen((const char *)wifi_pass));
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...PASS %s", wifi_config.sta.ssid, wifi_config.sta.password);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );

    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void tmp_task(void *pvParameters)
{
	while(1) {
		xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Got IP");
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		if (tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip_info) == 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip_info.ip));
            ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip_info.netmask));
            ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip_info.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }

		if (strlen((const char *)srv_ip) == 0) {
			ESP_LOGE(TAG, "expected server IP, reset board to setup again!");
			break;
		}

		if (srv_port == 0) {
			ESP_LOGE(TAG, "expected server port, reset board to setup again!");
			break;
		}

		// start espconn
		connect_server((const char *)srv_ip, srv_port);

		while(1) {
			vTaskDelay(5000 / portTICK_RATE_MS);
			//ESP_LOGI(TAG, "tmp_task loop");
		}
	}
	vTaskDelete(NULL);
}

void wifi_init_task(void)
{
	xTaskCreate(&tmp_task, "tmp_task", TMP_TASK_STACK_SIZE, NULL, TMP_TASK_PRIO, NULL);
}

void app_main()
{
	ESP_LOGI(TAG, "App started");
	show_copyright();
	
	list_init();
	ble_init();
	uart_init();

	wifi_event_group = xEventGroupCreate();

	show_help_info();
}