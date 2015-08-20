################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modules/modularity/compute_modularity.cpp \
../modules/modularity/test_modularity.cpp 

OBJS += \
./modules/modularity/compute_modularity.o \
./modules/modularity/test_modularity.o 

CPP_DEPS += \
./modules/modularity/compute_modularity.d \
./modules/modularity/test_modularity.d 


# Each subdirectory must supply rules for building sources it contributes
modules/modularity/%.o: ../modules/modularity/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


