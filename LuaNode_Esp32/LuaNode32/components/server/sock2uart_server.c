#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sock2uart_server.h"
#include "c_types.h"
#include "my_uart.h"
#include "wifi_init.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"

#define SOCK2UART_SERVER_TASK_PRIO	9
#define TCP_BUFF_SIZE				1024
#define MAX_TCP_CLIENT_CONN			1
#define TCP_PORT					9001

xTaskHandle tcp_srv_tsk_handle;

uint8 *recv_buf;

static void handle_receive(void *inFd)
{
	int32 client_sock = *(int *) inFd;
	int32 recbytes = 0;
	printf("tcp do_process > setup socket:(%d)\n", client_sock);

	while (1) {
		memset(recv_buf, 0, TCP_BUFF_SIZE);
		recbytes = read(client_sock, recv_buf, TCP_BUFF_SIZE);	// block
		if (recbytes > 0) {
			uart_sendStr((const char *)recv_buf);

		} else if (recbytes <= 0) {
			printf("tcp do_process > read less than 0 byte\n");
			break;
		}
	}
}

void tcp_server_task(void *pvParameters)
{
	int32 listenfd;
	int32 ret;

	struct sockaddr_in server_addr, remote_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_len = sizeof(server_addr);
	server_addr.sin_port = htons(TCP_PORT);

	recv_buf = (uint8 *) malloc(TCP_BUFF_SIZE);
	if (recv_buf == NULL) {
		printf("tcp_server_task buff alloc failed\n");
		vTaskDelete(NULL);
		return;
	}

	//step 1
	do {
		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd == -1) {
			printf("TCP server task > socket error\n");
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (listenfd == -1);
	printf("TCP server task > create socket: %d\n", listenfd);

	//step 2
	// Bind to the local port
	do {
		ret = bind(listenfd, (struct sockaddr * )&server_addr, sizeof(server_addr));
		if (ret != 0) {
			printf("TCP server task > bind fail\n");
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (ret != 0);
	printf("ESP8266 TCP server task > port:%d\n", ntohs(server_addr.sin_port));

	//step 3
	do {
		/* Listen to the local connection */
		ret = listen(listenfd, MAX_TCP_CLIENT_CONN);
		if (ret != 0) {
			printf("TCP server task > failed to set listen queue!\n");
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (ret != 0);
	printf("TCP server task > listen ok\n");

	//step 4
	int32 client_sock;
	int32 len = sizeof(struct sockaddr_in);
	for (;;) {
		printf("TCP server task > wait client\n");
		/*block here waiting remote connect request*/
		if ((client_sock = accept(listenfd, (struct sockaddr * )&remote_addr, (socklen_t * )&len)) < 0) {
			printf("TCP server task > accept fail\n");
			continue;
		}

		printf("TCP server task > Client from %s %d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));
		//xTaskCreate(task_tcp_server_client, "task_tcp_server_client", 512, &client_sock, 8, NULL);
		handle_receive(&client_sock);
		close(client_sock);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

	free(recv_buf);
	vTaskDelete(NULL);
}

static void tcp_server_start(void)
{
	xTaskCreate(tcp_server_task, "tcp_server_task", 1024, NULL, SOCK2UART_SERVER_TASK_PRIO, &tcp_srv_tsk_handle);
}

void sock2uart_server_start(void)
{
	printf("sock2uart server start\n");
	wifi_init();
	tcp_server_start();
}
