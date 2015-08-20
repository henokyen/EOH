################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/v1-testing/a.cpp \
../doc/build/v2/test/v1-testing/b.cpp \
../doc/build/v2/test/v1-testing/c.cpp 

OBJS += \
./doc/build/v2/test/v1-testing/a.o \
./doc/build/v2/test/v1-testing/b.o \
./doc/build/v2/test/v1-testing/c.o 

CPP_DEPS += \
./doc/build/v2/test/v1-testing/a.d \
./doc/build/v2/test/v1-testing/b.d \
./doc/build/v2/test/v1-testing/c.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/v1-testing/%.o: ../doc/build/v2/test/v1-testing/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


