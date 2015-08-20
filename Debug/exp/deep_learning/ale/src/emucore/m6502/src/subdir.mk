################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CXX_SRCS += \
../exp/deep_learning/ale/src/emucore/m6502/src/Device.cxx \
../exp/deep_learning/ale/src/emucore/m6502/src/M6502.cxx \
../exp/deep_learning/ale/src/emucore/m6502/src/M6502Hi.cxx \
../exp/deep_learning/ale/src/emucore/m6502/src/M6502Low.cxx \
../exp/deep_learning/ale/src/emucore/m6502/src/NullDev.cxx \
../exp/deep_learning/ale/src/emucore/m6502/src/System.cxx 

OBJS += \
./exp/deep_learning/ale/src/emucore/m6502/src/Device.o \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502.o \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502Hi.o \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502Low.o \
./exp/deep_learning/ale/src/emucore/m6502/src/NullDev.o \
./exp/deep_learning/ale/src/emucore/m6502/src/System.o 

CXX_DEPS += \
./exp/deep_learning/ale/src/emucore/m6502/src/Device.d \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502.d \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502Hi.d \
./exp/deep_learning/ale/src/emucore/m6502/src/M6502Low.d \
./exp/deep_learning/ale/src/emucore/m6502/src/NullDev.d \
./exp/deep_learning/ale/src/emucore/m6502/src/System.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/emucore/m6502/src/%.o: ../exp/deep_learning/ale/src/emucore/m6502/src/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


