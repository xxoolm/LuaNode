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
#include "lua.h"
#include "testrunner.h"
#include "spiffs_test_params.h"
#include "esp_spiffs.h"

enum {
	CMD_SPIFFS,
    CMD_END,
};

#define SSC_CMD_N   (CMD_END + 1)

LOCAL void spiffs_test_init(void);

LOCAL ssc_cmd_t sscCmdSet[SSC_CMD_N] =		{
    {"fs", CMD_T_SYNC,  CMD_SPIFFS, spiffs_test_init, NULL},
    {"",   CMD_T_ASYNC, CMD_END,    NULL,               NULL}
};

void spiffs_test_init(void)
{
    char *argv[10], pLine[128];
    int argc;

    strcpy(pLine, ssc_param_str());
    argc = ssc_parse_param(pLine, argv);

    run_tests(argc, argv);
}

void spiffs_test_help(void)
{
    printf("\nhelp:\n");
    printf("$ fs \n");
}

void spiffs_fs1_init(void)
{
    struct esp_spiffs_config config;

    config.phys_size = FS1_FLASH_SIZE;
    config.phys_addr = FS1_FLASH_ADDR;
    config.phys_erase_block = SECTOR_SIZE;
    config.log_block_size = LOG_BLOCK;
    config.log_page_size = LOG_PAGE;
    config.fd_buf_size = FD_BUF_SIZE * 2;
    config.cache_buf_size = CACHE_BUF_SIZE;

    esp_spiffs_init(&config);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    printf("SDK version:%s\n", system_get_sdk_version());

	char *lua_argv[] = {(char *)"lua", (char *)"-i", NULL};
    lua_main(2, lua_argv);

	spiffs_fs1_init();

	ssc_attach(SSC_BR_74880);
    ssc_register(sscCmdSet, SSC_CMD_N, spiffs_test_help);
}

