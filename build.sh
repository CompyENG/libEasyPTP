#!/bin/sh

# This script is responsible for building the libeasyptp shared library.

g++ -shared -fPIC -I./libeasyptp/ CameraBase.cpp CHDKCamera.cpp LVData.cpp PTPCamera.cpp PTPContainer.cpp PTPUSB.cpp -o libeasyptp.so -lusb-1.0

