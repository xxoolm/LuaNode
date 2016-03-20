#include "espressif/esp_common.h"
#include "uart.h"
#include "lua.h"
#include "FreeRTOS.h"
#include "task.h"


void user_init(void) {
    UART_SetBaudrate(0, 115200);
    printf("SDK Version:%s\n", sdk_system_get_sdk_version());
    
    // Create lua state, and then invoke lua function in lua.c (see detail in lua_main function)
    char *lua_argv[] = {(char *)"lua", (char *)"-i", NULL};
    lua_main(2, lua_argv);

}

