################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CXX_SRCS += \
../exp/deep_learning/ale/src/os_dependent/FSNodePOSIX.cxx \
../exp/deep_learning/ale/src/os_dependent/FSNodeWin32.cxx \
../exp/deep_learning/ale/src/os_dependent/OSystemUNIX.cxx \
../exp/deep_learning/ale/src/os_dependent/OSystemWin32.cxx \
../exp/deep_learning/ale/src/os_dependent/SettingsUNIX.cxx \
../exp/deep_learning/ale/src/os_dependent/SettingsWin32.cxx 

OBJS += \
./exp/deep_learning/ale/src/os_dependent/FSNodePOSIX.o \
./exp/deep_learning/ale/src/os_dependent/FSNodeWin32.o \
./exp/deep_learning/ale/src/os_dependent/OSystemUNIX.o \
./exp/deep_learning/ale/src/os_dependent/OSystemWin32.o \
./exp/deep_learning/ale/src/os_dependent/SettingsUNIX.o \
./exp/deep_learning/ale/src/os_dependent/SettingsWin32.o 

CXX_DEPS += \
./exp/deep_learning/ale/src/os_dependent/FSNodePOSIX.d \
./exp/deep_learning/ale/src/os_dependent/FSNodeWin32.d \
./exp/deep_learning/ale/src/os_dependent/OSystemUNIX.d \
./exp/deep_learning/ale/src/os_dependent/OSystemWin32.d \
./exp/deep_learning/ale/src/os_dependent/SettingsUNIX.d \
./exp/deep_learning/ale/src/os_dependent/SettingsWin32.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/os_dependent/%.o: ../exp/deep_learning/ale/src/os_dependent/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


