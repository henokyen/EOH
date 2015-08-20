################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../doc/build/v2/engine/boehm_gc/tests/leak_test.c \
../doc/build/v2/engine/boehm_gc/tests/middle.c \
../doc/build/v2/engine/boehm_gc/tests/test.c \
../doc/build/v2/engine/boehm_gc/tests/thread_leak_test.c 

CC_SRCS += \
../doc/build/v2/engine/boehm_gc/tests/test_cpp.cc 

OBJS += \
./doc/build/v2/engine/boehm_gc/tests/leak_test.o \
./doc/build/v2/engine/boehm_gc/tests/middle.o \
./doc/build/v2/engine/boehm_gc/tests/test.o \
./doc/build/v2/engine/boehm_gc/tests/test_cpp.o \
./doc/build/v2/engine/boehm_gc/tests/thread_leak_test.o 

C_DEPS += \
./doc/build/v2/engine/boehm_gc/tests/leak_test.d \
./doc/build/v2/engine/boehm_gc/tests/middle.d \
./doc/build/v2/engine/boehm_gc/tests/test.d \
./doc/build/v2/engine/boehm_gc/tests/thread_leak_test.d 

CC_DEPS += \
./doc/build/v2/engine/boehm_gc/tests/test_cpp.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/engine/boehm_gc/tests/%.o: ../doc/build/v2/engine/boehm_gc/tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

doc/build/v2/engine/boehm_gc/tests/%.o: ../doc/build/v2/engine/boehm_gc/tests/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


