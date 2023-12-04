# HOW TO BUILD LuaNode for ESP32
---------------------------------

* Download ESP-IDF souces from the link: [esp-idf](https://github.com/espressif/esp-idf), and then setup the environtment according to idf documents.

* Change directory to `LuaNode32`, and then compile the project by executing the command: `idf.py build`.

* If you want to utilize File System (SPIFFS) for LuaNode, you have to change the partition file for SPIFFS first. To setup SPIFFS, input command: `idf.py menuconfig`, then go to `Partition Table` menu to select the file named, `partition_luanode.csv`, on the `LuaNode32` directory as the used partition file.

# FIRMWARE
---------------------------------

The firmware we built in the firmware directory is for test. You can flash it 
to your device to test LuaNode. The flash address for the firmware we provided
is `0x1000`.
