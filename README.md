LuaNode
======================================

This project is an improved `Lua` SDK, based on ESP-IDF/ESP8266_RTOS_SDK, for Esp32 (compatible with Esp8266).

As we know, there are some existing Lua SDK such as NodeMCU, esp-open-rtos, but they are not support for both Esp32 at this time, and
some of them are not support multi-task.


![github](https://ksr-ugc.imgix.net/projects/2439759/photo-original.jpg?w=1024&h=576&fit=fill&bg=FFFFFF&v=1462164335&auto=format&q=92&s=e2d90d11a3fdbffa9d2b9ee5ac975d6a "Esp32 & Esp8266")

NEW UPDATE FOR ESP32:
--------------------------------------

New version of LuaNode is developed based on esp-idf and compatible with `Esplorer`.

![github](http://ww1.sinaimg.cn/mw690/999babe3gw1f9mtxsz3pzj20tv0jx11k.jpg "ESP32")

### How To Build LuaNode32
* Download build tools [Here](https://www.jianguoyun.com/p/DZYjgb4Q2bSDBhj8xx8 "toolchains")
* Export build tools' directory by executing the following command on terminal,
  `export PATH=/your_path/toolchains/esp32/bin:$PATH`, where the `your_path` is the
  path the toolchains stored.
* Export `esp-idf` path by the following command: `export IDF_PATH=the_esp-idf_path`
* Change current directory to `LuaNode32`, then input `make` to build firmware.

For more details, view LuaNode [wiki page](https://github.com/Nicholas3388/LuaNode/wiki "LuaNode Wiki")

LUA PROGRAMMING
--------------------------------------
Programming with Lua is easy, some samples are as follow:

```lua
-- create file on file system
file.open("myfile.lua", "w+");
file.write("hello world");
file.close();

-- read file from file system
file.open("myfile.lua", "r");
-- read 1024 bytes from myfile.lua and save them
-- to content
content=file.read(1024);
print(content);
file.close();

-- remove file
file.remove("myfile.lua");

-- restart device
node.restart();
```

You can add your own Lua modules to LuaNode at will, visit the LuaNode [wiki page](https://github.com/Nicholas3388/LuaNode/wiki "LuaNode Wiki") for more details.


HOW TO BUILD For `ESP32/ESP8266`:
--------------------------------------

For details, see LuaNode wiki page [Here](https://github.com/Nicholas3388/LuaNode/wiki/How-to-build-for-ESP8266-ESP32).

HOW TO FLASH THE FIRMWARE:
--------------------------------------

See LuaNode wiki page [Here](https://github.com/Nicholas3388/LuaNode/wiki/How-to-flash-the-firmware).

HOW TO DEBUG:
--------------------------------------

See wiki page [Here](https://github.com/Nicholas3388/LuaNode/wiki/How-to-debug).


APPS:
--------------------------------------

* `task`: This is a sample to show how to create an os task. Build the example by executing the gen_misc.sh.

How to create a task:

```c
void task1(void *pvParameters) {
    // do something
}

void user_init(void) {
    xTaskCreate(task1, (signed char *)"tsk1", 256, &mainqueue, 2, NULL);
}
```

`Note: There is a task to receive uart input. You'd better alloc more than 512k memory for this task,
since the lua command handler will be called in this task, more memory is required for lua handler.`

* `luaapp`: A lua app. 

* `wifikiller`: An wifi sample. Set baud rate to 115200pbs, using UART0. It will disconnect WiFi connection.

* `wifilister`: List all APs, along with clents connected to them, near your device. The list info then sent to Android device via OTG, and display on Android.

APPLICATIONS:
--------------------------------------

### Wifilister

Another interesting application is the `Wifilister` app. The app scans all APs along with the clients connected to them around device, and scan results are sent to
Android device via OTG, then you can see the result displayed on Android device.

To test this app, you have to install the `LuanodeUsbHost` Android app to your Android phone (device).
`LuanodeUsbHost` is an Android USB Host app for ESP8266/ESP32. The Android device receive messages, sent from ESP8266, via OTG.
Then the messages display on this app.

Download the `LuanodeUsbHost` source [Here](https://github.com/Nicholas3388/LuanodeUsbHost).

Compile `Wifilister` provided in Luanode, flash it to ESP8266/ESP32, and then connect your ESP8266/ESP32 with Android phone.
You can see the scanning results.

![github](http://ww1.sinaimg.cn/mw690/999babe3jw1f6cwovhg2vj207i0dct9g.jpg "LuanodeUsbHost")

FAQ:
--------------------------------------

See wiki page [Here](https://github.com/Nicholas3388/LuaNode/wiki/FAQ)


WHO ARE THE PEOPLE BEHIND THIS PROJECT
-------------------------------------

`Wei Wang`: He got his master's degree from Guilin University of Electronic Technology in 2012. He has more than 4 years work experience on Embedded System.

![github](https://ksr-ugc.imgix.net/assets/007/539/619/4b02fe2896172cf177b6558300cdfb45_original.jpg?w=680&fit=max&v=1461463286&auto=format&q=92&s=7d405bf015bc1b0036851755b03663dc)

`DOIT`(www.doit.am): DOIT is a high-tech company, who focus on IoT technology and open source hardware. He supports this project and provides hardware for this project. Its successful products include WifiMCU, Doit Video Car, and Wifi sniffer, etc.

![github](https://ksr-ugc.imgix.net/assets/010/012/333/2e153bf8a6f1cc5781643ad8d704f285_original.gif?w=680&fit=max&v=1461661228&q=92&s=c8f52001f353a41b455f2a55b3c11256)


CONTACT ME:
--------------------------------------

If you have any question, you can contact me via Email/QQ list below, Thanks:) 

  - `Email`: nicholas3388@gmail.com
  - `QQ`: 535873968


CHANGE LOG:
--------------------------------------
2016.4.7 
	Modified lbaselib.c

2016.4.19
	Modified code structure	

2016.5.11
    Replace libc
