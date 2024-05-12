#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "uart_handler.h"
#include "lua.h"
#include "utils.h"

#define TXD_PIN (GPIO_NUM_7) //UART_NUM_0_TX: gpio1
#define RXD_PIN (GPIO_NUM_6) //UART_NUM_0_RX: gpio3
#define USE_UART	(UART_NUM_1)

#define ECHO_TASK_STACK_SIZE    (4096)
#define RX_BUF_SIZE 			1024

static const char *TAG = "uart";

static uint8_t pbuff[RX_BUF_SIZE] = {0};

static RcvMsgBuff rcvMsgBuff;

bool uart_getc(char *c)
{
    if(rcvMsgBuff.pWritePos == rcvMsgBuff.pReadPos){   // empty
        return false;
    }
    //ets_intr_lock();
    *c = (char)*(rcvMsgBuff.pReadPos);
    if (rcvMsgBuff.pReadPos == (rcvMsgBuff.pRcvMsgBuff + RX_BUF_SIZE)) {
        rcvMsgBuff.pReadPos = rcvMsgBuff.pRcvMsgBuff ; 
    } else {
        rcvMsgBuff.pReadPos++;
    }
    //ets_intr_unlock();
    return true;
}

static void echo_task(void *arg)
{
	//ESP_LOGI(TAG, "My uart init\n");
	if (rcvMsgBuff.pRcvMsgBuff != NULL) {
		free(rcvMsgBuff.pRcvMsgBuff);
	}
	rcvMsgBuff.pRcvMsgBuff = malloc(RX_BUF_SIZE);
	memset(rcvMsgBuff.pRcvMsgBuff, 0, RX_BUF_SIZE);
	rcvMsgBuff.pWritePos = rcvMsgBuff.pRcvMsgBuff;
	rcvMsgBuff.pReadPos = rcvMsgBuff.pRcvMsgBuff;
	
	while(1) {
		int len = uart_read_bytes(USE_UART, pbuff, (RX_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
		//uart_write_bytes(USE_UART, (const char *) (pbuff), len);
		if (len > 0) {
			for (int i = 0; i < len; i++) {
				*(rcvMsgBuff.pWritePos) = pbuff[i];
				//ESP_LOGI(TAG, "uart char:%d", pbuff[i]);
				if (rcvMsgBuff.pWritePos == (rcvMsgBuff.pRcvMsgBuff + RX_BUF_SIZE)) {
					// overflow ...we may need more error handle here.
					rcvMsgBuff.pWritePos = rcvMsgBuff.pRcvMsgBuff ;
				} else {
					rcvMsgBuff.pWritePos++;
				}
			}
			/*if (len == 2 && pbuff[0] == 0x0d && pbuff[1] == 0x0a) { // received '\r\n' from ESPlore, response with '\r\n>' 
				uint8_t resp[4] = {0x0d, 0x0a, 0x3e, 0x00};
				uart_write_bytes(USE_UART, resp, 3);
				continue;
			} */
			lua_handle_input(false);
		}
	}
	
	if (rcvMsgBuff.pRcvMsgBuff != NULL) {
		free(rcvMsgBuff.pRcvMsgBuff);
	}
	ESP_LOGE(TAG, "Error, uart task is dead!");
	vTaskDelete(NULL);
}

void my_uart_init(void) 
{
	//ESP_LOGI(TAG, "My uart init\n");
	const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(USE_UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(USE_UART, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(USE_UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	
	xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
