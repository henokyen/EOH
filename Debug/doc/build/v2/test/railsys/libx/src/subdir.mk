################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/railsys/libx/src/test_libx.cpp 

OBJS += \
./doc/build/v2/test/railsys/libx/src/test_libx.o 

CPP_DEPS += \
./doc/build/v2/test/railsys/libx/src/test_libx.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/railsys/libx/src/%.o: ../doc/build/v2/test/railsys/libx/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


