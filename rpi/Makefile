#############################################################################
#
# Makefile for librf24 examples on Raspberry Pi
#
# License: GPL (General Public License)
# Author:  gnulnulf <arco@appeltaart.mine.nu>
# Date:    2013/02/07 (version 1.0)
#
# Description:
# ------------
# use make all and make install to install the examples
# You can change the install directory by editing the prefix line
#
HW=$(shell uname -m)
include Makefile.$(HW)

prefix := /usr/local
headers := -I ../../RF24/librf24-rpi/librf24/

# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Wall -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
#CCFLAGS=

# define all programs
PROGRAMS = hub

all: hub

RoomState.h: ../arduino/SensorNode/RoomState.h
	cp ../arduino/SensorNode/RoomState.h .

RoomState.cpp: ../arduino/SensorNode/RoomState.cpp
	cp ../arduino/SensorNode/RoomState.cpp .

hub : hub.o RoomState.o
	$(CXX) ${CCFLAGS} ${headers} -L../../RF24/librf24-rpi/librf24/  -lrf24 hub.o RoomState.o -o hub
	
hub.o : hub.cpp RoomState.h
	$(CXX) -c ${CCFLAGS} ${headers}  hub.cpp -o $@
	
RoomState.o : RoomState.cpp RoomState.h
	$(CXX) -c ${CCFLAGS} ${headers}  RoomState.cpp -o $@
	
clean:
	rm -rf $(PROGRAMS) RoomState.* hub.o

install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/bin || mkdir $(prefix)/bin
	for prog in $(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/bin; \
	done

.PHONY: install
