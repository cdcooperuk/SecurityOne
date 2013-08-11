################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/colin/Dropbox/arduino/sketchbook/libraries/RF24/RF24.cpp 

OBJS += \
./RF24/RF24.o 

CPP_DEPS += \
./RF24/RF24.d 


# Each subdirectory must supply rules for building sources it contributes
RF24/RF24.o: /home/colin/Dropbox/arduino/sketchbook/libraries/RF24/RF24.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/usr/share/arduino/hardware/arduino/cores/arduino" -I"/usr/share/arduino/hardware/arduino/variants/standard" -I"/home/colin/Dropbox/arduino/sketchbook/SensorNode" -I"/usr/share/arduino/libraries/SPI" -I"/home/colin/Dropbox/arduino/sketchbook/libraries/RF24" -D__IN_ECLIPSE__=1 -DUSB_VID= -DUSB_PID= -DARDUINO=105 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


