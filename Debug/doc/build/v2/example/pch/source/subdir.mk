################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/example/pch/source/hello_world.cpp 

OBJS += \
./doc/build/v2/example/pch/source/hello_world.o 

CPP_DEPS += \
./doc/build/v2/example/pch/source/hello_world.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/example/pch/source/%.o: ../doc/build/v2/example/pch/source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


