################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/example/qt/qt4/hello/arrow.cpp \
../doc/build/v2/example/qt/qt4/hello/main.cpp 

OBJS += \
./doc/build/v2/example/qt/qt4/hello/arrow.o \
./doc/build/v2/example/qt/qt4/hello/main.o 

CPP_DEPS += \
./doc/build/v2/example/qt/qt4/hello/arrow.d \
./doc/build/v2/example/qt/qt4/hello/main.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/example/qt/qt4/hello/%.o: ../doc/build/v2/example/qt/qt4/hello/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


