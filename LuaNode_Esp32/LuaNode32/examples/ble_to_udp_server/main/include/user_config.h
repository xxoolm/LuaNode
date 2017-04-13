#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define UDP_TASK_STACK_SIZE			2048
#define UDP_TASK_PRIO				12
#define UART_TASK_STACK_SIZE		2048
#define UART_TASK_PRIO				11
#define UART_EVT_TASK_STACK_SIZE	2048
#define UART_EVT_TASK_PRIO			10
#define TMP_TASK_STACK_SIZE			2048
#define TMP_TASK_PRIO				9
#define BUF_SIZE					1024
#define RECV_BUF_SIZE				128
#define WIFI_SSID_MAX_LEN			32
#define WIFI_PASS_MAX_LEN			32
#define SRV_IP_MAX_LEN				16

#define BDA_SIZE					16

#define PROMPT						"\r\n"
#define PROMPT_LEN					2

#endif
