# Component makefile for extras/mylibc

INC_DIRS += $(mylibc_ROOT)include

# args for passing into compile rule generation
mylibc_INC_DIR =  $(mylibc_ROOT)
mylibc_SRC_DIR =  $(mylibc_ROOT)

$(eval $(call component_compile_rules,mylibc))
