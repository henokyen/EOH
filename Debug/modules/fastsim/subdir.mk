################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modules/fastsim/display.cpp \
../modules/fastsim/fastsim_keyboard.cpp \
../modules/fastsim/laser.cpp \
../modules/fastsim/light_sensor.cpp \
../modules/fastsim/linear_camera.cpp \
../modules/fastsim/main.cpp \
../modules/fastsim/map.cpp \
../modules/fastsim/radar.cpp \
../modules/fastsim/robot.cpp \
../modules/fastsim/test_fastsim.cpp 

OBJS += \
./modules/fastsim/display.o \
./modules/fastsim/fastsim_keyboard.o \
./modules/fastsim/laser.o \
./modules/fastsim/light_sensor.o \
./modules/fastsim/linear_camera.o \
./modules/fastsim/main.o \
./modules/fastsim/map.o \
./modules/fastsim/radar.o \
./modules/fastsim/robot.o \
./modules/fastsim/test_fastsim.o 

CPP_DEPS += \
./modules/fastsim/display.d \
./modules/fastsim/fastsim_keyboard.d \
./modules/fastsim/laser.d \
./modules/fastsim/light_sensor.d \
./modules/fastsim/linear_camera.d \
./modules/fastsim/main.d \
./modules/fastsim/map.d \
./modules/fastsim/radar.d \
./modules/fastsim/robot.d \
./modules/fastsim/test_fastsim.d 


# Each subdirectory must supply rules for building sources it contributes
modules/fastsim/%.o: ../modules/fastsim/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


