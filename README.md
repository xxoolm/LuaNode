## ESP-LUA-RTOS-SDK

A `lua` `rtos` sdk, based on esp-open-rtos-sdk, for esp8266/esp32.


======================================
## HOW TO BUILD:

Windows:

* We utilize VirtualBox (VB) with Ubuntu installed as build environment. If your OS is Linux, you can also setup the build environment according the following steps:
  - 1. Install VirtualBox-5.0.16-105871-Win (You'd better install the latest version, since I found that the older version such as 4.x cannot install the VBox Guest Additions)
  - 2. Download ubuntu-15.10-desktop-amd64.iso and the install it on VB. Make sure that asign more than 1G RAM and 12G Hard Disk for this virtual machine. The tool chain build process require much memory!
  - 3. Setup share file folder: Settings->Share Folders->Add new shared folder, input your share folder path and the select `Make Permanent`, Note that `do not` select Auto Mount. Launch Ubuntu and then create a folder to mount the share folder, I create a folder, named `share`, in the path /mnt. Type the commond in the terminal to mount share folder:

```sh
sudo mount -t vboxsf share /mnt/share 
```

Note: if you mount failed, check that is `vboxsf` exist. Type common ```sh lsmod | grep vboxsf ``` in the terminal. If nothing found, maybe the `Guest Additions` are not installed correctly.


Linux:

* Use Virtual Box:
  - 1. System requiement: RAM>1G; Hard Disk>10G
  - 2. Setup build environment as that on Windows:)

Mac OS:

* The build environment setup as that on Linux

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
