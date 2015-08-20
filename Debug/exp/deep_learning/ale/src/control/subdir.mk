################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../exp/deep_learning/ale/src/control/ALEState.cpp \
../exp/deep_learning/ale/src/control/fifo_controller.cpp \
../exp/deep_learning/ale/src/control/game_controller.cpp \
../exp/deep_learning/ale/src/control/internal_controller.cpp 

OBJS += \
./exp/deep_learning/ale/src/control/ALEState.o \
./exp/deep_learning/ale/src/control/fifo_controller.o \
./exp/deep_learning/ale/src/control/game_controller.o \
./exp/deep_learning/ale/src/control/internal_controller.o 

CPP_DEPS += \
./exp/deep_learning/ale/src/control/ALEState.d \
./exp/deep_learning/ale/src/control/fifo_controller.d \
./exp/deep_learning/ale/src/control/game_controller.d \
./exp/deep_learning/ale/src/control/internal_controller.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/control/%.o: ../exp/deep_learning/ale/src/control/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


