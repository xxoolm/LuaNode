#!/bin/bash

:<<!
******NOTICE******
MUST set SDK_PATH & BIN_PATH & APP_NAME firstly!!!
example:
export SDK_PATH=~/LuaNode_Esp32
export BIN_PATH=~/LuaNode_Esp32_Bin
!

export SDK_PATH=/mnt/Share/LuaNode/LuaNode/LuaNode_Esp32
export BIN_PATH=/mnt/Share/LuaNode/LuaNode/LuaNode_Esp32_Bin
export APP_NAME=wifilister

echo "gen_misc.sh version 20151105"
echo ""

if [ $SDK_PATH ]; then
    echo "SDK_PATH:"
    echo "$SDK_PATH"
    echo ""
else
    echo "ERROR: Please export SDK_PATH in gen_misc.sh firstly, exit!!!"
    exit
fi

if [ $BIN_PATH ]; then
    echo "BIN_PATH:"
    echo "$BIN_PATH"
    echo ""
else
    echo "ERROR: Please export BIN_PATH in gen_misc.sh firstly, exit!!!"
    exit
fi

echo ""
echo "start..."
echo ""

make clean

make
