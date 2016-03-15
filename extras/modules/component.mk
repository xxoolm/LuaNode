# Component makefile for extras/modules

INC_DIRS += $(modules_ROOT)include

# args for passing into compile rule generation
modules_INC_DIR =  $(modules_ROOT)
modules_SRC_DIR =  $(modules_ROOT)

$(eval $(call component_compile_rules,modules))
