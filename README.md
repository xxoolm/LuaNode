LuaNode
======================================

This project is an improved `Lua` SDK, based on ESP-IDF/ESP8266_RTOS_SDK, for Esp32 (compatible with Esp8266).

As we know, there are some existing Lua SDK such as NodeMCU, esp-open-rtos, but they are not support for both Esp32 at this time, and
some of them are not support multi-task.


![github](https://ksr-ugc.imgix.net/projects/2439759/photo-original.jpg?w=1024&h=576&fit=fill&bg=FFFFFF&v=1462164335&auto=format&q=92&s=e2d90d11a3fdbffa9d2b9ee5ac975d6a "Esp32 & Esp8266")

NEW UPDATE FOR ESP32:
--------------------------------------

New version of LuaNode32 is developed based on esp-idf and compatible with `Esplorer`.

The following figures show the DOIT ESP32 dev-board. To buy the ESP32 dev-board click [Here](https://item.taobao.com/item.htm?spm=a312a.7700824.w4002-7420449993.9.yduA0V&id=542459608596)

 
![github](https://img.alicdn.com/imgextra/i1/116050204/TB2jI_AfXOP.eBjSZFHXXXQnpXa_!!116050204.jpg "ESP32 dev-board")
![github](https://img.alicdn.com/imgextra/i3/116050204/TB20VbCfhaK.eBjSZFwXXXjsFXa_!!116050204.jpg "ESP32 dev-board back")

Download the DOIT dev-board schematic [here](https://www.dropbox.com/s/jefwxxtufgwg0ex/esp32_Schematic%20Prints.pdf?dl=0 "schematic")

### How To Build LuaNode32
* Download prebuild toolchain [Here](https://github.com/jmattsson/nodemcu-prebuilt-toolchains "toolchains"), you 
  can also build the tools from source, follow the steps [Here](http://esp-idf.readthedocs.io/en/latest/linux-setup.html "How to build toolchain")
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

LuaNode is compatible with `Esplorer` now, you can edit and download Lua
Code to ESP32 with `Esplorer` conveniently.

Get Esplorer [Here](https://github.com/4refr0nt/ESPlorer "Esplorer")

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

### ESP32-Camera
This simple application is to use ESP32 to controll a Camera. You can find the source code from the following diretory: _LuaNode_Esp32/LuaNode32/examples/camera_

There is another ESP32 camera demo create by _igrr_, find it from the link: https://github.com/igrr/esp32-cam-demo. 
We also utilize OV7725 for our camera test, but the hardware is a little different from _igrr_'s project. 
My camera is extended with a RAM, which is AL422B, with 390KB RAM. So the camera can cache 2 frames, if the 
resolution is set to 320x240 pixel. The following figure is my camera, called "Yehuo", a Chinese name.

![github](http://img2.ph.126.net/MdqGO0DRTakEUyYiGUw8Lg==/6632360791372792649.jpg "Yehuo Camera")

The following figures is my ESP32-Camera.

![github](http://img2.ph.126.net/z5YqIBznWy0Ha0YUbZ8uCw==/6631869309677845517.jpg "ESP32-Camera")

![github](http://img2.ph.126.net/4ImKqI0vy7uPq_DGjUmk9w==/6632299218721636202.jpg "ESP32-Camera")

I learn _igrr_'s project, and do the same test as _igrr_'s. For instance, I use the camera to scan the board, and then 
you can see the output from the uart terminal.


![github](http://img1.ph.126.net/n_FzfADuecVypWxR494Gyg==/6632216755352253574.jpg "Esp32 camera test")

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

  - `Email`: 535873968@qq.com
  - `QQ`: 535873968


CHANGE LOG:
--------------------------------------
2016.4.7 
	Modified lbaselib.c

2016.4.19
	Modified code structure	

2016.5.11
    Replace libc

2017.1.19
	Update to the latest esp-idf
