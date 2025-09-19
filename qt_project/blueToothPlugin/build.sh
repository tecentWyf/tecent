#!/bin/bash

export PATH=/opt/arm/opt/ext-toolchain/bin/:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export QTDIR=/usr/local/Qt-5.9.0
export PATH=$QTDIR/bin:$PATH
export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH

qmake blueToothPlugin.pro