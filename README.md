LuaNode
======================================

This project is an improved `Lua` SDK, based on ESP32-RTOS-SDK/ESP8266_RTOS_SDK, for Esp32 (compatible with Esp8266).

As we know, there are some existing Lua SDK such as NodeMCU, esp-open-rtos, but them are not support for both Esp32 at this time, and
some of them are not support multi-task.

Our kickstarter campaign is launched. If you back for our project, an Esp32 dev-kit will ship to you once the project goal is completed.
The hardware is provided by www.doit.am.

Our Kickstarter campaign is alive: https://www.kickstarter.com/projects/857552561/luanode-for-esp8266-and-esp32 . If you back for it, we will be very appreciated!

![github](https://ksr-ugc.imgix.net/projects/2439759/photo-original.jpg?w=1024&h=576&fit=fill&bg=FFFFFF&v=1462164335&auto=format&q=92&s=e2d90d11a3fdbffa9d2b9ee5ac975d6a "Esp32 & Esp8266")

PROJECT STRUCTRUE:
--------------------------------------
```
LuaNode-master
    -->LuaNode_Esp32
        -->bin
        -->components
            -->apps
		-->luaapp
		-->task
		-->wifikiller
            -->driver
            -->include
            -->libc
            -->lua
            -->modules
            -->platform
        -->extra_include
        -->include
        -->ld
        -->lib
        -->third_party
        -->tools
    -->LuaNode_Esp8266
        -->The structure is almost the same as that of esp32
```

* `bin` contains blank.bin, boot.bin.

* `components` contains all components the project needed, including driver, modules, and lua source, etc.

* `extra_include` contains the extra head files for this project.

* `include` is a directory that contains head files for this project.

* `third_party` contains head files for the libraries used, such as freertos, in this project.

* `ld` contains the link files.

* `lib` contains some libraries to be linked when bin file generated.

* `tools` contains some useful tools.

Note that the wifikiller app only support ESP8266.

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
file.read("myfile.lua");
file.close();

-- remove file
file.remove("myfile.lua");

-- restart device
node.restart();
```

You can add your own Lua modules to LuaNode at will, visit the LuaNode [wiki page](https://github.com/Nicholas3388/LuaNode/wiki "LuaNode Wiki") for detail.


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

* `luaapp`: A lua sample. 

* `wifikiller`: An wifi sample. Set baud rate to 115200pbs, using UART0. It will disconnect WiFi connection.


APPLICATIONS:
--------------------------------------

A funny application is to utilize Luanode and Esp32/Esp8266 to create a Wifi tank. You can control the tank via app installed on your phone.
The Luanode run on Esp32/Esp8266 and wait for the commands sent from phone, and then drives the motor on tank to move it on.

![github](http://ww4.sinaimg.cn/mw690/999babe3gw1f3b37i18lej20fe0duq7f.jpg "Appearance")

The hardware contains: T300 Tank Chassis, Esp8266 Development Kit, 720p HD Camera, WR703N Wireless Router.

![github](http://ww4.sinaimg.cn/mw690/999babe3gw1f3b37iabgvj20kt0c6jv7.jpg "Hardware")

A video on Youtube to illustrate how to control the tank via phone.

[![WiFi video car](http://a1.qpic.cn/psb?/V14H7R7s11nbTN/CUnAapb.4U2AgDuGfsAWrztFuYKc2TE.tvJ**Kshk4c!/b/dKgAAAAAAAAA&bo=QAHwAAAAAAAFB5U!&rf=viewer_4)](https://www.youtube.com/watch?v=9SpL22obIMw&feature=youtu.be)

Another video on Youku, a Chinese video website, to illustrate the features of the tank.

[![WiFi video car](http://a1.qpic.cn/psb?/V14H7R7s11nbTN/CUnAapb.4U2AgDuGfsAWrztFuYKc2TE.tvJ**Kshk4c!/b/dKgAAAAAAAAA&bo=QAHwAAAAAAAFB5U!&rf=viewer_4)](http://v.youku.com/v_show/id_XMTI5NjUwOTYzMg==.html?from=s1.8-1-1.2)



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
  - `Homepage`: www.apparduino.com


CHANGE LOG:
--------------------------------------
2016.4.7 
	Modified lbaselib.c

2016.4.19
	Modified code structure	

2016.5.11
    Replace libc
