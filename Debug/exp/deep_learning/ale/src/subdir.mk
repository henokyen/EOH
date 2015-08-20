################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../exp/deep_learning/ale/src/main.cpp 

OBJS += \
./exp/deep_learning/ale/src/main.o 

CPP_DEPS += \
./exp/deep_learning/ale/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/%.o: ../exp/deep_learning/ale/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


