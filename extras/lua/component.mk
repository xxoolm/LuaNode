# Component makefile for extras/lua

INC_DIRS += $(lua_ROOT)include

# args for passing into compile rule generation
lua_INC_DIR =  $(lua_ROOT)
lua_SRC_DIR =  $(lua_ROOT)

$(eval $(call component_compile_rules,lua))
