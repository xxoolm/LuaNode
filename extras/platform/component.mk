# Component makefile for extras/platform

INC_DIRS += $(platform_ROOT)include

# args for passing into compile rule generation
platform_INC_DIR =  $(platform_ROOT)
platform_SRC_DIR =  $(platform_ROOT)

$(eval $(call component_compile_rules,platform))
