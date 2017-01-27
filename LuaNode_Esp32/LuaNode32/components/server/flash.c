/*
 * flash.c
 *
 *  Created on: 2016年11月30日
 *      Author: Administrator
 */

#include "flash.h"
#include "esp_common.h"
#include "platform.h"
#include "user_config.h"
#include "user_interface.h"
#include "esp_spi_flash.h"
#include "sock2uart_config.h"
#include "rom/uart.h"
#include "driver/uart.h"
#include "my_uart.h"

#define FLASH_UNIT_SIZE		4
#define LOG_PAGE_SIZE       FLASH_READ_SIZE//256

flash_config fc;
flash_config *pfc = NULL;
_CONFIG_PARA ESP_WORD_ALIGN cfgPara;

static s32_t esp_readwrite(u32_t addr, u32_t size, u8_t *p, int write)
{
	/*if(size > LOG_PAGE_SIZE) {
		printf("esp_readwrite size is larger than LOG_PAGE_SIZE\n");
		return -1;
	}*/
    char tmp_buf[LOG_PAGE_SIZE + FLASH_UNIT_SIZE * 2];
    u32_t aligned_addr = addr & (-FLASH_UNIT_SIZE);
    u32_t aligned_size = ((size + (FLASH_UNIT_SIZE - 1)) & -FLASH_UNIT_SIZE) + FLASH_UNIT_SIZE;

    int res = spi_flash_read(aligned_addr, (u32_t *) tmp_buf, aligned_size);

    if (res != 0) {
        printf("spi_flash_read failed: %d (%d, %d)\n\r", res, (int) aligned_addr, (int) aligned_size);
        return res;
    }

    if (!write) {
        memcpy(p, tmp_buf + (addr - aligned_addr), size);
        return 0;
    }

    memcpy(tmp_buf + (addr - aligned_addr), p, size);

    res = spi_flash_write(aligned_addr, (u32_t *) tmp_buf, aligned_size);

    if (res != 0) {
	    printf("spi_flash_write failed: %d (%d, %d)\n\r", res, (int) aligned_addr, (int) aligned_size);
        return res;
    } else {
    	printf("spi_flash_write ok\n");
    }

    return 0;
}

static s32_t write_flash(u32_t addr, u32_t len, u8_t *str)
{
	return esp_readwrite(addr, len, str, 1);
}

static s32_t read_flash(u32_t addr, u32_t len, u8_t *buff)
{
	return esp_readwrite(addr, len, buff, 0);
	//uint32_t t;
	//spi_flash_read(FLASH_START_ADDR, &t, sizeof(t));
	//printf("==> res = %d\n", t);
}

static s32_t erase_sector(u32_t addr)
{
    /*
     * With proper configurarion spiffs always
     * provides here sector address & sector size
     */
    if (addr % SPI_FLASH_SEC_SIZE != 0) {
        printf("Invalid size provided to esp_spiffs_erase (%d)\n\r", (int) addr);
        return SPI_FLASH_RESULT_ERR;
    }

    return spi_flash_erase_sector(addr /SPI_FLASH_SEC_SIZE);
}

static s32_t erase_flash(u32_t addr, u32_t size)
{
	u32_t rest = 0;
	u32_t percent = 0;
	u32_t tmp_addr = addr;
	if (size%SPI_FLASH_SEC_SIZE > 0) { rest = 1;}
	u32_t sec_num = (int)(size/SPI_FLASH_SEC_SIZE) + rest;
	u32_t count = 0;
	printf("Flash erase start \n");
	while (erase_sector(tmp_addr + count*SPI_FLASH_SEC_SIZE) == 0) {
		//printf(".");
		count++;
		percent = (u32_t)(count*100/sec_num);
		printf("%d%%\n", percent);
		if (count == sec_num) {
			printf("\nFinish erasing\n");
			return SPI_FLASH_RESULT_OK;
		}
	}
	return SPI_FLASH_RESULT_ERR;
}

void flash_init(void)
{
	printf("flash init\n");
	u32_t phy_start_addr = ( u32_t )platform_flash_get_first_free_block_address( NULL );
	phy_start_addr += 0x3FFF;
	phy_start_addr &= 0xFFFFC000;  // align to 4 sector.

	u32_t phy_size = platform_get_flash_size(phy_start_addr);
	u32_t phy_end_addr = phy_start_addr + phy_size;
	printf("fs.start:0x%08x, end:0x%08x, max:0x%08x\n",phy_start_addr, phy_end_addr, phy_size);

	fc.phy_start = phy_start_addr;
	fc.phy_end = phy_end_addr;
	fc.phy_size = phy_size;
	fc.hal_read_f = read_flash;
	fc.hal_write_f = write_flash;
	fc.hal_erase_f = erase_flash;

	pfc = &fc;
}

flash_config *flash_get_config(void)
{
	/*
	 * Singleton mode
	 * Called flash_init() previously, otherwise flash initialize here
	 */
	if (pfc == NULL) {
		flash_init();
	}
	return pfc;
}

void config_read(void)
{
	int len = sizeof(cfgPara);
	flash_config *fc = flash_get_config();
	if (SPI_FLASH_RESULT_OK != fc->hal_read_f(SPI_FLASH_PARA_ADDR, len, (u8_t *) &cfgPara)) {
		printf("\n spi_flash_read secotor: len:%d failed!!!", len);
		return;
	}
}

void config_save(void)
{
	int len = sizeof(cfgPara);
	flash_config *fc = flash_get_config();
	fc->hal_erase_f(SPI_FLASH_PARA_ADDR, 2048);
	if (SPI_FLASH_RESULT_OK != fc->hal_write_f(SPI_FLASH_PARA_ADDR, len, (u8_t *) &cfgPara)) {
		printf("config write failed\n");
	}
}

void config_restore(void)
{
	cfgPara.magic_num = MAGIC_NUM;
	cfgPara.baud = BIT_RATE_9600;
	cfgPara.bits = UART_WordLength_8b ;
	cfgPara.parity = UART_PARITY_DISABLE;
	cfgPara.stop = USART_StopBits_1;
	cfgPara.apE = 1;
	cfgPara.apEncrypt = AUTH_OPEN;
	strcpy(cfgPara.apN, "Kinco_WiFi");
	cfgPara.apP[0]=0x00;
	strcpy(cfgPara.apIP, "192.168.4.1");
	strcpy(cfgPara.apNM, "255.255.255.0");
	strcpy(cfgPara.apGW, "192.168.4.1");
	cfgPara.staE = 0;
	strcpy(cfgPara.staN, "Wireless_Router");
	strcpy(cfgPara.staP, "");
	cfgPara.dhcpE = 1;
	strcpy(cfgPara.staIP, "192.168.1.1");
	strcpy(cfgPara.staNM, "255.255.255.0");
	strcpy(cfgPara.staGW, "192.168.1.1");
	cfgPara.net_mode = 0;//0:tcp server;1: tcp client; 2: udp server;3 udp client
	strcpy(cfgPara.tcprip, "192.168.1.100");
	cfgPara.tcprport = 6000;
	cfgPara.tcplport = 9000;
	strcpy(cfgPara.udprip, "192.168.1.100");
	cfgPara.udprport = 6000;
	cfgPara.udplport = 9000;
	cfgPara.rs485 = 100;//默认不开启
	cfgPara.uart_time = 100;//默认100ms uart分包间隔
	cfgPara.split_time = 0;//默认不使能分包判断
	cfgPara.uart_log = false;//默认禁止分包判断
	config_save();
}

uint8 read_rom_uint8(const uint8* addr)
{
    uint32 bytes;
    bytes = *(uint32*)((uint32)addr & ~3);
    return ((uint8*)&bytes)[(uint32)addr & 3];
}

void read_rom_flash(char *src, char*des, int len)
{
	int i=0;
	for(i=0;i <len; i++) {
		des[i] = (char)read_rom_uint8(src+i);
	}
	des[len]=0x00;
}