# Component makefile for extras/spiffs

INC_DIRS += $(spiffs_ROOT)include

# args for passing into compile rule generation
spiffs_INC_DIR =  $(spiffs_ROOT)
spiffs_SRC_DIR =  $(spiffs_ROOT)

$(eval $(call component_compile_rules,spiffs))
