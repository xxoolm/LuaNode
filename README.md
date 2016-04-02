## ESP-LUA-RTOS-SDK

A `lua` `rtos` sdk, based on esp-open-rtos-sdk, for esp8266/esp32.


======================================
## HOW TO BUILD:

## Windows:

* We utilize VirtualBox (VB) with Ubuntu installed as build environment. If your OS is Linux, you can also setup the build environment according the following steps:
  - 1. Install VirtualBox-5.0.16-105871-Win (You'd better install the latest version, since I found that the older version such as 4.x cannot install the VBox Guest Additions)
  - 2. Download ubuntu-15.10-desktop-amd64.iso and the install it on VB. Make sure that asign more than 1G RAM and 12G Hard Disk for this virtual machine. The tool chain build process require much memory!
  - 3. Setup share file folder: 

## Linux:

* Use Virtual Box:
  - 1. System requiement: RAM>1G; Hard Disk>10G
  - 2. Setup build environment as that on Windows:)

* Setup Build Environtment:
  - 1. Replace the esptool.py in esp-open-sdk/xtensa-elf and esp-open-sdk/bin with the new esptool.py from esptool-master directory.
  - 2. Secondly, 

## Mac OS:

======================================
## EXAMPLES:

* simple: This is a sample to show how to create an os task. Build the example by typing `make` or `make rebuild` in the example directory.

* lua_test: A lua sample. 

* terminal: A uart sample.

======================================
## CONTACT ME: 
  - Email: nicholas3388@gmail.com
  - QQ: 535873968

======================================
## REFERENCE:

* NodeMCU: https://github.com/nodemcu/nodemcu-firmware

* esp-open-rtos: https://github.com/SuperHouse/esp-open-rtos

* ESP8266-RTOS-SDK: https://github.com/espressif/ESP8266_RTOS_SDK

* esptool: https://github.com/themadinventor/esptool

* GDBSTUB: https://github.com/espressif/esp-gdbstub

* lua: www.lua.org

* elua: www.eluaproject.net

* WifiMCU: www.wifimcu.com/en.html
