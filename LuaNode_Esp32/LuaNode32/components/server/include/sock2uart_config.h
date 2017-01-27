#ifndef _SOCK2UART_CONFIG_H_
#define _SOCK2UART_CONFIG_H_

#define ESP_WORD_ALIGN __attribute__ ((aligned (4)))
#define MAGIC_NUM 0xA0

typedef struct
{
  uint16 magic_num;
  uint32 baud;
  char bits;
  char parity;
  char stop;
  uint8 rs485;//为100的时候关闭rs485功能，否则就是管脚号

  char apE;
  char apEncrypt; //encrypt method
  char apN[33];
  char apP[65];
  char apIP[16];
  char apGW[16];
  char apNM[16];

  char staE; //enable sta or not
  char staN[33];
  char staP[65];
  char dhcpE;
  char staIP[16];
  char staGW[16];
  char staNM[16];

  char net_mode;  //0:tcp server, 1:tcp client, 2:udp server, 3:udp client
  char tcprip[65]; //remote tcp ip
  uint tcprport;	//remote tcp port
  uint tcplport;	//local tcp server port
  char udprip[65];	//remote tcp ip
  uint udprport;	//remote tcp port
  uint udplport;	//local tcp server port
  int uart_time;//uart接收时间间隔     (修改为整形，防止出现xTaskGetTickCount() - last_uart_tick=-1的情况)
  int split_time;//modbus RTU超时时间 =0停用 >0
  bool uart_log;//启用串口log
} _CONFIG_PARA;

#endif