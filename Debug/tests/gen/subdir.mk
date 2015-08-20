################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/gen/bit_string.cpp \
../tests/gen/cmaes.cpp \
../tests/gen/evo_float.cpp \
../tests/gen/sampled.cpp 

OBJS += \
./tests/gen/bit_string.o \
./tests/gen/cmaes.o \
./tests/gen/evo_float.o \
./tests/gen/sampled.o 

CPP_DEPS += \
./tests/gen/bit_string.d \
./tests/gen/cmaes.d \
./tests/gen/evo_float.d \
./tests/gen/sampled.d 


# Each subdirectory must supply rules for building sources it contributes
tests/gen/%.o: ../tests/gen/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


