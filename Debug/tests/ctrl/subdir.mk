################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/ctrl/agent.cpp 

OBJS += \
./tests/ctrl/agent.o 

CPP_DEPS += \
./tests/ctrl/agent.d 


# Each subdirectory must supply rules for building sources it contributes
tests/ctrl/%.o: ../tests/ctrl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


