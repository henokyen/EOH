################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../exp/deep_learning/ale/src/common/Defaults.cpp \
../exp/deep_learning/ale/src/common/export_screen.cpp 

CXX_SRCS += \
../exp/deep_learning/ale/src/common/RectList.cxx \
../exp/deep_learning/ale/src/common/SoundNull.cxx \
../exp/deep_learning/ale/src/common/SoundSDL.cxx 

OBJS += \
./exp/deep_learning/ale/src/common/Defaults.o \
./exp/deep_learning/ale/src/common/RectList.o \
./exp/deep_learning/ale/src/common/SoundNull.o \
./exp/deep_learning/ale/src/common/SoundSDL.o \
./exp/deep_learning/ale/src/common/export_screen.o 

CPP_DEPS += \
./exp/deep_learning/ale/src/common/Defaults.d \
./exp/deep_learning/ale/src/common/export_screen.d 

CXX_DEPS += \
./exp/deep_learning/ale/src/common/RectList.d \
./exp/deep_learning/ale/src/common/SoundNull.d \
./exp/deep_learning/ale/src/common/SoundSDL.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/common/%.o: ../exp/deep_learning/ale/src/common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

exp/deep_learning/ale/src/common/%.o: ../exp/deep_learning/ale/src/common/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


