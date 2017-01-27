/*
 * spi_flash.h
 *
 *  Created on: 2016Äê11ÔÂ29ÈÕ
 *      Author: Administrator
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include "c_types.h"

#define SPI_FLASH_PARA_ADDR	0x110000

typedef s32_t (*flash_read)(u32_t addr, u32_t size, u8_t *dst);
typedef s32_t (*flash_write)(u32_t addr, u32_t size, u8_t *src);
typedef s32_t (*flash_erase)(u32_t addr, u32_t size);

typedef struct {
	uint32_t 	phy_start;
	uint32_t 	phy_end;
	uint32_t 	phy_size;
	flash_read 	hal_read_f;
	flash_write hal_write_f;
	flash_erase hal_erase_f;
} flash_config;

void flash_init(void);
flash_config *flash_get_config(void);
void config_read(void);
void config_save(void);
void config_restore(void);
void read_rom_flash(char *src, char*des, int len);

#endif /* INCLUDE_SPI_FLASH_H_ */
