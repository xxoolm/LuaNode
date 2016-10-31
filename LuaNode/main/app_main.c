
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "my_uart.h"
#include "lua.h"
#include "flash_fs.h"

void init_task(void *pvParameters)
{
	printf("task init\n");
	int mount_res = fs_init();
	printf("mount result: %d\n", mount_res);
	/** add your init code here **/

	//do_luainit();
    vTaskDelete(NULL);
}

void app_main()
{
	uart_init();
    //xTaskCreatePinnedToCore(&pingTask, "pingTask", 512, NULL, 5, NULL, 0);
	xTaskCreate(init_task, "init_task", 1024, NULL, 11, NULL);

	char *lua_argv[] = {(char *)"lua", (char *)"-i", NULL};
    lua_main(2, lua_argv);
}

