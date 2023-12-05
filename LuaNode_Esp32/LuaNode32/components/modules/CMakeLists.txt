idf_component_register(SRCS "node.c" "file.c" "gpio.c"
					REQUIRES lua utils spiffs driver
					INCLUDE_DIRS "include")
