# Component makefile for extras/driver

INC_DIRS += $(driver_ROOT)include

# args for passing into compile rule generation
driver_INC_DIR =  $(driver_ROOT)
driver_SRC_DIR =  $(driver_ROOT)

$(eval $(call component_compile_rules,driver))
