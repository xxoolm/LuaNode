# LuaNode for w800

***

LuaNode_W800 is a Lua develop kit for WinnerMicro w80x serial WiFi+Bt module, it aims at fast develop with Lua program°£

Features:
* Fast develop£ª
* Ease to undstand;
* Upgrade without erasing and flashing your chip£ª
* Running on other LuaNode platform, such as ESP32, ESP8266, and Stm32L4 (Coming soon);

Lua is a powerful, efficient, lightweight, embeddable scripting language. It supports procedural programming, object-oriented programming, functional programming, data-driven programming, and data description.


LuaNode startup:
![boot](https://raw.githubusercontent.com/Nicholas3388/LuaNodeForW800/master/LuaNode_W800/img/boot.png)

## (1) Hardware
The hardware is a dev-board utilized w800 module, named »Û∫ÕNeptune(W800), which shows as the following figure:

![product](https://raw.githubusercontent.com/Nicholas3388/LuaNodeForW800/master/LuaNode_W800/img/product.png)

### GPIO
GPIO for the w800 dev-board shows as below:

![board](https://raw.githubusercontent.com/Nicholas3388/LuaNodeForW800/master/LuaNode_W800/img/board.png)

![pin](https://raw.githubusercontent.com/Nicholas3388/LuaNodeForW800/master/LuaNode_W800/img/pin.png)

## (2) Lua programming

In the LuaNode kit, the w800 peripheral drivers are written with C, packaging into libraries so as to be invoked easily°£For example, the following Lua code shows how to blink a LED on the dev-board£∫

```lua
require "wmtime"
require "wmgpio"

voltage = HIGH

blink = function()
  if voltage == HIGH then
    voltage = LOW
    wmgpio.out(PORTB, 8, LOW)
  else 
    voltage = HIGH
    wmgpio.out(PORTB, 8, HIGH)
  end
end

wmgpio.init(PORTB, 8, GPIO_OUT, FLOATING)
timerId = wmtime.create(1000, REPEAT, "blink")
wmtime.start(timerId)
```

## (3) Other features

Ongoing ...