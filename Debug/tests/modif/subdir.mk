################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/modif/diversity.cpp 

OBJS += \
./tests/modif/diversity.o 

CPP_DEPS += \
./tests/modif/diversity.d 


# Each subdirectory must supply rules for building sources it contributes
tests/modif/%.o: ../tests/modif/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


