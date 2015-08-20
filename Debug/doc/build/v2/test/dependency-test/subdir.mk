################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/dependency-test/a.cpp \
../doc/build/v2/test/dependency-test/b.cpp \
../doc/build/v2/test/dependency-test/c.cpp \
../doc/build/v2/test/dependency-test/e.cpp 

C_SRCS += \
../doc/build/v2/test/dependency-test/a_c.c 

OBJS += \
./doc/build/v2/test/dependency-test/a.o \
./doc/build/v2/test/dependency-test/a_c.o \
./doc/build/v2/test/dependency-test/b.o \
./doc/build/v2/test/dependency-test/c.o \
./doc/build/v2/test/dependency-test/e.o 

C_DEPS += \
./doc/build/v2/test/dependency-test/a_c.d 

CPP_DEPS += \
./doc/build/v2/test/dependency-test/a.d \
./doc/build/v2/test/dependency-test/b.d \
./doc/build/v2/test/dependency-test/c.d \
./doc/build/v2/test/dependency-test/e.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/dependency-test/%.o: ../doc/build/v2/test/dependency-test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

doc/build/v2/test/dependency-test/%.o: ../doc/build/v2/test/dependency-test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


