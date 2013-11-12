#!/bin/bash

avrdude -v -v -v -v -patmega328p -cusbtiny -Uflash:w:SensorNode.hex:i 
