#include <stdio.h>
#include <string.h>

#include "udp_task.h"
#include "lwip/api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "utils.h"
#include "my_list.h"

#define LOCAL_PORT		9999

extern void ble_client_app_register(void);

static const char *TAG = "udp_task";

static struct netconn *conn;

static void ip_str2num(const char *ip, int *res)
{
	int len = strlen(ip), i = 0;
	int start = 0;
	for (i = 0; i < len; i++) {
		if (ip[i] == '.') {
			*res = str2num((ip+start), (i-start));
			res++;
			start = i+1;
			ESP_LOGI(TAG, "%s", ip+start);
		}
	}
	// last value
	*res = str2num((ip+start), (i-start));
}

void connect_server(const char *ip, int port)
{
	conn = netconn_new(NETCONN_UDP);
	ip_addr_t serverip;
	int ip_val[4] = {0};
	ip_str2num(ip, ip_val);
	ESP_LOGI(TAG, "IP -> %d.%d.%d.%d", ip_val[0], ip_val[1], ip_val[2], ip_val[3]);
	IP_ADDR4(&serverip, ip_val[0], ip_val[1], ip_val[2], ip_val[3]);

	if (conn == NULL) {
		ESP_LOGE(TAG, "Create conn failed");
		return; 
	}
	netconn_bind(conn, NULL, LOCAL_PORT);
	err_t er;
	while ((er = netconn_connect(conn, &serverip, port)) != ERR_OK) {
		ESP_LOGE(TAG, "Connect server failed, error code: %d, try again 1 seconds", er);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	ESP_LOGI(TAG, "Connected to UDP server");
	send_data((const char *)"test", 4);

	//ble_client_app_register();

	// send BLE scan results one by one 
	char buff[24] = {0};
	scan_list_t *h = list_get_head();
	scan_list_t *iterator = h->pNext;
	while (iterator != NULL) {
		memset(buff, 0, 24);
		sprintf(buff, "%s, %d\n", iterator->bda, iterator->rssi);
		send_data((const char *)buff, strlen((const char *)buff));
		ESP_LOGI(TAG, "%s", buff);
		iterator = iterator->pNext;
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	list_destroy();
}

void send_data(const char *data, int len)
{
	struct netbuf *TCPNetbuf =netbuf_new();
	netbuf_ref(TCPNetbuf, data, len);
	netconn_send(conn,TCPNetbuf);
	netbuf_free(TCPNetbuf);
}

void disconnect_server()
{
	// not implement
}