COMPONENT_ADD_INCLUDEDIRS := include

CFLAGS += -DdefaultThreadStack=1024

include $(IDF_PATH)/make/component_common.mk
