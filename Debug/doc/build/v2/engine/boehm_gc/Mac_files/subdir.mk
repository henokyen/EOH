################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../doc/build/v2/engine/boehm_gc/Mac_files/dataend.c \
../doc/build/v2/engine/boehm_gc/Mac_files/datastart.c 

OBJS += \
./doc/build/v2/engine/boehm_gc/Mac_files/dataend.o \
./doc/build/v2/engine/boehm_gc/Mac_files/datastart.o 

C_DEPS += \
./doc/build/v2/engine/boehm_gc/Mac_files/dataend.d \
./doc/build/v2/engine/boehm_gc/Mac_files/datastart.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/engine/boehm_gc/Mac_files/%.o: ../doc/build/v2/engine/boehm_gc/Mac_files/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


