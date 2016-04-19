/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"

uint8 *RcvMsgBuff;
uint8 *pRcvMsgBuff;
uint8 *pWritePos;
uint8 *pReadPos;


LOCAL STATUS ICACHE_FLASH_ATTR uart_tx_one_char(uint8 uart, uint8 TxChar)
{
    while (true) {
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }

    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    return OK;
}

LOCAL void uart0_rx_intr_handler(void *para)
{
    bool got_input = false;
    uint8 RcvChar;
    uint8 fifo_len = 0;
    uint8 buf_idx = 0;
    uint32 uart_intr_status = READ_PERI_REG(UART_INT_ST(UART0));

    while (uart_intr_status != 0x0) {
	if (UART_FRM_ERR_INT_ST == (uart_intr_status & UART_FRM_ERR_INT_ST)) {
            //printf("FRM_ERR\r\n");
            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_FRM_ERR_INT_CLR);
        } else if (UART_RXFIFO_FULL_INT_ST == (uart_intr_status & UART_RXFIFO_FULL_INT_ST)) {
            printf("full\r\n");   // overflow
            fifo_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
            buf_idx = 0;

            while (buf_idx < fifo_len) {
                uart_tx_one_char(UART0, READ_PERI_REG(UART_FIFO(UART0)) & 0xFF);
                buf_idx++;
            }

            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
        } else if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST)) {
            //printf("tout\r\n");
            RcvChar = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
            *pWritePos = RcvChar;
            if (RcvChar == 0x10 || RcvChar == 0x13 || RcvChar == 0x0d ) {      // is '\r' or '\n'
                printf("\r\n>>");
                uart_tx_one_char(UART0, 0x4d);      // move the prompt toward right
                uart_tx_one_char(UART0, 0xe0);
            }
            
            if (RcvChar != 0x08 && RcvChar != 0x10 && RcvChar != 0x13) {
                uart_tx_one_char(UART0, RcvChar);
            }
                
            if (pWritePos == (RcvMsgBuff + RX_BUFF_SIZE)) {
                // overflow ...we may need more error handle here.
                pWritePos = pRcvMsgBuff ;
            } else {
                pWritePos++;
            }
        
            if (pWritePos == pReadPos){   // overflow one byte, need push pReadPos one byte ahead
                if (pReadPos == (pRcvMsgBuff + RX_BUFF_SIZE)) {
                    pReadPos = pRcvMsgBuff; 
                } else {
                    pReadPos++;
                }
            }
        
            //uart_tx_one_char(UART0, READ_PERI_REG(UART_FIFO(UART0)) & 0xFF);
            got_input = true;
            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
        } else if (UART_TXFIFO_EMPTY_INT_ST == (uart_intr_status & UART_TXFIFO_EMPTY_INT_ST)) {
            printf("empty\n\r");
            WRITE_PERI_REG(UART_INT_CLR(UART0), UART_TXFIFO_EMPTY_INT_CLR);
            CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
        } else {
            //skip
        }

	uart_intr_status = READ_PERI_REG(UART_INT_ST(UART0));
    }
    
    if (got_input) {
        //printf("got input");
        if (readline()) {
            dojob();
        }
    }
    
    printf("fuck you\n");
}

void ICACHE_FLASH_ATTR luaInitTask(void *pvParameters)
{
    printf("Init lua here\n");

    RcvMsgBuff = (uint8 *)malloc(RX_BUFF_SIZE);
    memset(RcvMsgBuff, 0, RX_BUFF_SIZE);
    pWritePos = RcvMsgBuff;
    pReadPos = RcvMsgBuff;
    pRcvMsgBuff = RcvMsgBuff;

    vTaskDelete(NULL);
}

void ICACHE_FLASH_ATTR uart_init()
{
    //Serial Configuration
    UART_SetBaudrate(0, BIT_RATE_115200);

    UART_IntrConfTypeDef uart_intr;
    uart_intr.UART_IntrEnMask = UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA | UART_RXFIFO_FULL_INT_ENA | UART_TXFIFO_EMPTY_INT_ENA;
    uart_intr.UART_RX_FifoFullIntrThresh = RX_BUFF_SIZE;
    uart_intr.UART_RX_TimeOutIntrThresh = 2;
    uart_intr.UART_TX_FifoEmptyIntrThresh = 20;
    UART_IntrConfig(UART0, &uart_intr);

    // UART receive message handler
    UART_intr_handler_register(uart0_rx_intr_handler, NULL);
    ETS_UART_INTR_ENABLE();
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_init(void)
{
    printf("SDK version:%s\n", system_get_sdk_version());
    
    uart_init();
    
    char* lua_argv[] = { (char *)"lua", (char *)"-i", NULL };
    lua_main( 2, lua_argv );
    
    xTaskCreate(luaInitTask, "luaInitTask", 256, NULL, 2, NULL);
}

