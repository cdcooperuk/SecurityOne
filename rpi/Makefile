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

all: hub tester

hub : hub.o RoomState.o
	$(CXX) ${CCFLAGS} ${headers} -L../../RF24/librf24-rpi/librf24/  -lrf24 hub.o RoomState.o -o hub
	
tester : tester.o 
	$(CXX) ${CCFLAGS} ${headers} -L../../RF24/librf24-rpi/librf24/  -lrf24 tester.o -o tester
	
hub.o : hub.cpp ../SOcommon/RoomState.h
	$(CXX) -c ${CCFLAGS} ${headers}  hub.cpp -o $@

tester.o : tester.cpp
	$(CXX) -c ${CCFLAGS} ${headers}  tester.cpp -o $@
	
	
RoomState.o : ../SOcommon/RoomState.cpp ../SOcommon/RoomState.h
	$(CXX) -c ${CCFLAGS} ${headers}  ../SOcommon/RoomState.cpp -o $@
	
clean:
	rm -rf $(PROGRAMS) RoomState.* hub.o tester.o

install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/bin || mkdir $(prefix)/bin
	for prog in $(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/bin; \
	done

.PHONY: install
