################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/direct-request-test/a.cpp \
../doc/build/v2/test/direct-request-test/b.cpp \
../doc/build/v2/test/direct-request-test/b_inverse.cpp 

OBJS += \
./doc/build/v2/test/direct-request-test/a.o \
./doc/build/v2/test/direct-request-test/b.o \
./doc/build/v2/test/direct-request-test/b_inverse.o 

CPP_DEPS += \
./doc/build/v2/test/direct-request-test/a.d \
./doc/build/v2/test/direct-request-test/b.d \
./doc/build/v2/test/direct-request-test/b_inverse.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/direct-request-test/%.o: ../doc/build/v2/test/direct-request-test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


