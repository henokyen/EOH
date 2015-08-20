################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../exp/deep_learning/ale/src/games/RomSettings.cpp \
../exp/deep_learning/ale/src/games/RomUtils.cpp \
../exp/deep_learning/ale/src/games/Roms.cpp 

OBJS += \
./exp/deep_learning/ale/src/games/RomSettings.o \
./exp/deep_learning/ale/src/games/RomUtils.o \
./exp/deep_learning/ale/src/games/Roms.o 

CPP_DEPS += \
./exp/deep_learning/ale/src/games/RomSettings.d \
./exp/deep_learning/ale/src/games/RomUtils.d \
./exp/deep_learning/ale/src/games/Roms.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/games/%.o: ../exp/deep_learning/ale/src/games/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


