################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modules/modularity/grc/file_handler.cpp \
../modules/modularity/grc/mother.cpp \
../modules/modularity/grc/network.cpp \
../modules/modularity/grc/node.cpp 

OBJS += \
./modules/modularity/grc/file_handler.o \
./modules/modularity/grc/mother.o \
./modules/modularity/grc/network.o \
./modules/modularity/grc/node.o 

CPP_DEPS += \
./modules/modularity/grc/file_handler.d \
./modules/modularity/grc/mother.d \
./modules/modularity/grc/network.d \
./modules/modularity/grc/node.d 


# Each subdirectory must supply rules for building sources it contributes
modules/modularity/grc/%.o: ../modules/modularity/grc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


