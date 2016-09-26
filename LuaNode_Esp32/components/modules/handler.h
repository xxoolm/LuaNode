/**
 * cJSON for Lua
 * June 2016
 * Wang Wei
 **/

#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "cJSON.h"

#define FLOAT_ENABLE
#define DBG_PRINT       printf
#define DEFAULT_STRING  "default"


enum ADD_TO_TYPE {
    JSON_OBJECT,
    JSON_ARRAY
};

/*enum BOOL {
    FALSE = 0,
    TRUE = 1
};

#ifndef BOOL
#define BOOL    int
#endif // BOOL
*/
typedef struct node {
    cJSON *json_obj;
    struct node *next;
} node_t;

typedef struct list {
    struct node *head;
    struct node *tail;
    int length;
} list_t;


// prototypes
int json_decode(lua_State *l);
int json_encode(lua_State *l);

#endif // HANDLER_H
