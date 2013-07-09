#!/bin/sh

# Note: you'll probably need to run this as root.

cp libeasyptp.so /usr/lib/
mkdir /usr/include/libeasyptp/
cp ./libeasyptp/*.hpp /usr/include/libeasyptp/
cp libeasyptp.hpp /usr/include/
mkdir /usr/include/libeasyptp/chdk/
cp chdk/* /usr/include/libeasyptp/chdk/
