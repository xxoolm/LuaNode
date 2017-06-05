## Introduction

![github](https://thumbnail0.baidupcs.com/thumbnail/14c9908d7b0bf51a453b4dc649fc39f4?fid=1443503420-250528-85189969679319&time=1496545200&rt=sh&sign=FDTAER-DCb740ccc5511e5e8fedcff06b081203-YuIdOTg93EQhO88w4Q16EKAXLEU%3D&expires=8h&chkv=0&chkbd=0&chkpc=&dp-logid=3582790967995923504&dp-callid=0&size=c710_u400&quality=100 "OTA TCP update")

Although there is an OTA example provided by esp-idf, the new firmware download from http server, I found
that some times the connection is failed when firmware updating. In this sample, we provide a TCP method to 
update firmware via TCP server. If a package is sent failed, the client (ESP32) will request the server to
send package again.

* The request sent by client will have a string format like this: `req0\r\n\r\n` or `req1\r\n\r\n`, where "req0" 
  means client request server send next package, "req1" means there are some errors occur when client receive
  package, request server to send previous package again.

* The package sent by server will have the following format: `total: 123456\r\nlength: 321\r\n\r\n$#&*@#$......`, 
  where "total: 123456\r\nlength: 321\r\n\r\n$#&*@#$......" is package header, there are two key and value. 
  the key "total" refers to the total length of this firmware, the other key "length" refers to current package 
  length. With these information, we can show update progress to user.

## How to use

* Modify the SSID and password in `user_config.h` in client source. Then build client and 
  flash the generated firmware to your ESP32 device

* Store your new firmware, which used for updating and the `ota_server.py` on your laptop, 
  the two file must in the same folder, 
  then run the server on your laptop (You must install python on you laptop first), note that 
  the laptop and ESP32 must connect to the same AP.

* when ESP32 powerup, it will connect to the python server run on your laptop and download 
  the new firmware.

You can run the `ota_server.py` on remote server and store your new firmware on the remote 
server for updating. Just change the server IP and PORT defined in `user_config.h`
