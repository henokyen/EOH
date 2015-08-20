################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../exp/Normalized_hierarchy/network_toolbox-master/src/auto_bliss.cpp \
../exp/Normalized_hierarchy/network_toolbox-master/src/compute_modularity.cpp \
../exp/Normalized_hierarchy/network_toolbox-master/src/opt_placement.cpp 

OBJS += \
./exp/Normalized_hierarchy/network_toolbox-master/src/auto_bliss.o \
./exp/Normalized_hierarchy/network_toolbox-master/src/compute_modularity.o \
./exp/Normalized_hierarchy/network_toolbox-master/src/opt_placement.o 

CPP_DEPS += \
./exp/Normalized_hierarchy/network_toolbox-master/src/auto_bliss.d \
./exp/Normalized_hierarchy/network_toolbox-master/src/compute_modularity.d \
./exp/Normalized_hierarchy/network_toolbox-master/src/opt_placement.d 


# Each subdirectory must supply rules for building sources it contributes
exp/Normalized_hierarchy/network_toolbox-master/src/%.o: ../exp/Normalized_hierarchy/network_toolbox-master/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


