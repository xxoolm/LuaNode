
#include "esp_common.h"
#include "lua.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "c_stdio.h"


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    printf("SDK version:%s\n", system_get_sdk_version());

	uart_init_new();

	char *lua_argv[] = {(char *)"lua", (char *)"-i", NULL};
    lua_main(2, lua_argv);

	//wifi_station_disconnect();
	//wifi_set_opmode(NULL_MODE);
}
