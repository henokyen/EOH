################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/example/qt/qt4/uic/main.cpp 

OBJS += \
./doc/build/v2/example/qt/qt4/uic/main.o 

CPP_DEPS += \
./doc/build/v2/example/qt/qt4/uic/main.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/example/qt/qt4/uic/%.o: ../doc/build/v2/example/qt/qt4/uic/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


