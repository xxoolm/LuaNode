#ifndef MY_UART_H
#define MY_UART_H

#include "c_types.h"

//#define REG_UART_BASE( i )  (0x60000000+(i)*0x10000)
#define UART_INT_ST( i )                        (REG_UART_BASE( i ) + 0x8)


void uart_init(void);
void uart_sendStr(const char *str);
void uart_putc(uint8_t TxChar);

#endif