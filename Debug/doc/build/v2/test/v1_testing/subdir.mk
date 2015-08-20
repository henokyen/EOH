################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/v1_testing/foo.cpp \
../doc/build/v2/test/v1_testing/lib-err.cpp \
../doc/build/v2/test/v1_testing/lib.cpp 

OBJS += \
./doc/build/v2/test/v1_testing/foo.o \
./doc/build/v2/test/v1_testing/lib-err.o \
./doc/build/v2/test/v1_testing/lib.o 

CPP_DEPS += \
./doc/build/v2/test/v1_testing/foo.d \
./doc/build/v2/test/v1_testing/lib-err.d \
./doc/build/v2/test/v1_testing/lib.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/v1_testing/%.o: ../doc/build/v2/test/v1_testing/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


