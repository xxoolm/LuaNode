#include "my_uart.h"
#include "esp_intr.h"
#include "c_types.h"
#include "rom/uart.h"
#include "soc/uart_register.h"
#include "extras/esp_intr_ext.h"
#include "c_string.h"
#include "lua.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <stdio.h>
#include <stdlib.h>

#define UART_FRM_ERR_INT_ST (BIT(3))
#define UART_FRM_ERR_INT_CLR (BIT(3))
#define UART_INT_CLR( i )                       (REG_UART_BASE( i ) + 0x10)
#define UART_FIFO( i )                          (REG_UART_BASE( i ) + 0x0)
#define UART_RXFIFO_FULL_INT_ST (BIT(0))
#define UART_RXFIFO_FULL_INT_CLR (BIT(0))
#define UART_RXFIFO_TOUT_INT_ST (BIT(8))
#define UART_RXFIFO_TOUT_INT_CLR (BIT(8))
#define UART_TXFIFO_EMPTY_INT_CLR (BIT(1))
#define UART_TXFIFO_EMPTY_INT_ENA (BIT(1))
#define UART_INT_ENA( i )                       (REG_UART_BASE( i ) + 0xC)
#define UART_TXFIFO_EMPTY_INT_ST (BIT(1))
#define UART_RXFIFO_OVF_INT_CLR (BIT(4))
#define UART_RXFIFO_OVF_INT_ST (BIT(4))

enum {
    UART_EVENT_RX_CHAR,
    UART_EVENT_MAX
};

typedef struct _os_event_ {
    uint32 event;
    uint32 param;
} os_event_t;

xTaskHandle xUartTaskHandle;
xQueueHandle xQueueUart;
RcvMsgBuff rcvMsgBuff;

static void fs_init0(void)
{
	//int mount_res = fs_init();
	//os_printf("mount result: %d\n", mount_res);

	//int status = do_luainit();
}

void uart_task(void *pvParameters)
{
	// Close wifi temporary
	//char *appName = getenv("APP_NAME");
	//wifi_station_disconnect();
	//wifi_set_opmode(NULL_MODE);
	
	fs_init0();

    os_event_t e;

    for (;;) {
		//vTaskDelay(2000 / portTICK_PERIOD_MS);
        if (xQueueReceive(xQueueUart, &e, (portTickType)portMAX_DELAY)) {
            switch (e.event) {
                case UART_EVENT_RX_CHAR:
				{
					lua_handle_input(false);
				}
                    break;

                default:
                    break;
            }
        }
    }

	////////////////////////////////
	// program will never run here.

	//fs_deinit();

    vTaskDelete(NULL);
}

void uart0_rx_intr_handler(void *para)
{
	uint8 RcvChar;
	BaseType_t xHigherPriorityTaskWoken;
	uint32 uart_intr_status = READ_PERI_REG(UART_INT_ST(0)) ;

    while (uart_intr_status != 0x0) {
        if (UART_FRM_ERR_INT_ST == (uart_intr_status & UART_FRM_ERR_INT_ST)) {
            //uart_tx_one_char('!');
            WRITE_PERI_REG(UART_INT_CLR(0), UART_FRM_ERR_INT_CLR);
        } else if (UART_RXFIFO_FULL_INT_ST == (uart_intr_status & UART_RXFIFO_FULL_INT_ST)) {
			//uart_tx_one_char('$');
			RcvChar = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
            WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR);

			*(rcvMsgBuff.pWritePos) = RcvChar;

			if (RcvChar == '\r' || RcvChar == '\n' ) {
				rcvMsgBuff.BuffState = WRITE_OVER;
			}

			if (rcvMsgBuff.pWritePos == (rcvMsgBuff.pRcvMsgBuff + RX_BUFF_SIZE)) {
				// overflow ...we may need more error handle here.
				rcvMsgBuff.pWritePos = rcvMsgBuff.pRcvMsgBuff ;
			} else {
				rcvMsgBuff.pWritePos++;
			}

			if (rcvMsgBuff.pWritePos == rcvMsgBuff.pReadPos){   // overflow one byte, need push pReadPos one byte ahead
				if (rcvMsgBuff.pReadPos == (rcvMsgBuff.pRcvMsgBuff + RX_BUFF_SIZE)) {
					rcvMsgBuff.pReadPos = rcvMsgBuff.pRcvMsgBuff ; 
				} else {
					rcvMsgBuff.pReadPos++;
				}
			}

			os_event_t e;
			e.event = UART_EVENT_RX_CHAR;
			e.param = '+';
			xQueueSendFromISR(xQueueUart, &e, &xHigherPriorityTaskWoken);
			if( xHigherPriorityTaskWoken ) {
				// Actual macro used here is port specific.
				portYIELD_FROM_ISR ();
			}
		} else if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST)) {
            //uart_tx_one_char('%');
			RcvChar = READ_PERI_REG(UART_FIFO(0)) & 0xFF;
            WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_TOUT_INT_CLR);
        } else if (UART_TXFIFO_EMPTY_INT_ST == (uart_intr_status & UART_TXFIFO_EMPTY_INT_ST)) {
            //uart_tx_one_char('^');
            WRITE_PERI_REG(UART_INT_CLR(0), UART_TXFIFO_EMPTY_INT_CLR);
            CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_TXFIFO_EMPTY_INT_ENA);
        } else if (UART_RXFIFO_OVF_INT_ST  == (READ_PERI_REG(UART_INT_ST(0)) & UART_RXFIFO_OVF_INT_ST)) {
            WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_OVF_INT_CLR);
            printf("RX OVF!!\r\n");
        } else {
            //skip
        }

        uart_intr_status = READ_PERI_REG(UART_INT_ST(0)) ;
	}
}

void uart_sendStr(const char *str)
{
    while(*str) {
        uart_tx_one_char(*str++);
    }
}

void uart_putc(uint8_t TxChar)
{
    while (true) {
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS_REG(0)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }

    WRITE_PERI_REG(UART_FIFO(0) , TxChar);
}


void uart_init(void)
{
	rcvMsgBuff.pRcvMsgBuff = malloc(RX_BUFF_SIZE);
	memset(rcvMsgBuff.pRcvMsgBuff, 0, RX_BUFF_SIZE);
	rcvMsgBuff.pWritePos = rcvMsgBuff.pRcvMsgBuff;
	rcvMsgBuff.pReadPos = rcvMsgBuff.pRcvMsgBuff;

	ESP_UART0_INTR_DISABLE();
	ESP_UART0_INTR_ATTACH(uart0_rx_intr_handler, NULL);
	ESP_UART0_INTR_ENABLE();
	xQueueUart = xQueueCreate(32, sizeof(os_event_t));
	xTaskCreate(uart_task, "uart_task", 6144, NULL, 10, &xUartTaskHandle);
}