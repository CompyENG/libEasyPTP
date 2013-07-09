#####################################################################
#
# Copyright 2013 Bobby Graese <bobby.graese@gmail.com>
# 
# This file is part of libEasyPTP.
#
#  libEasyPTP is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  libEasyPTP is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with libEasyPTP.  If not, see
#  <http://www.gnu.org/licenses/>.
#
#####################################################################

# User configurable:
PREFIX := /usr/

# Probably shouldn't touch unless you know what you're doing:
CXX := g++
CP := cp
RM := rm -rf
MKDIR := mkdir -p
CXXFLAGS := -fPIC -Wall
LIBS := -lusb-1.0
INCLUDES := -I./include/
SRCS := ./lib/CameraBase.cpp \
		./lib/CHDKCamera.cpp \
		./lib/LVData.cpp \
		./lib/PTPCamera.cpp \
		./lib/PTPContainer.cpp \
		./lib/PTPUSB.cpp
		
OBJS := $(SRCS:%.cpp=%.o)
		
.PHONY: all install depend clean

all: depend libeasyptp.so

install: all
	$(CP) libeasyptp.so $(PREFIX)lib/
	$(MKDIR) $(PREFIX)include/libeasyptp/chdk/
	$(CP) -r ./include/* $(PREFIX)include/libeasyptp/
	
clean:
	$(RM) libeasyptp.so $(OBJS) .depend

libeasyptp.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c -o $@ $<

depend: .depend

.depend: $(SRCS)
	@$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $^ > ./.depend;

include .depend
