COMPONENT_LDFLAGS += -u GPIO_module_selectedLUA_USE_MODULES_GPIO
COMPONENT_ADD_INCLUDEDIRS := include

include $(IDF_PATH)/make/component_common.mk
