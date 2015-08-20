################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../doc/build/v2/engine/modules/order.c \
../doc/build/v2/engine/modules/path.c \
../doc/build/v2/engine/modules/property-set.c \
../doc/build/v2/engine/modules/regex.c \
../doc/build/v2/engine/modules/sequence.c \
../doc/build/v2/engine/modules/set.c 

OBJS += \
./doc/build/v2/engine/modules/order.o \
./doc/build/v2/engine/modules/path.o \
./doc/build/v2/engine/modules/property-set.o \
./doc/build/v2/engine/modules/regex.o \
./doc/build/v2/engine/modules/sequence.o \
./doc/build/v2/engine/modules/set.o 

C_DEPS += \
./doc/build/v2/engine/modules/order.d \
./doc/build/v2/engine/modules/path.d \
./doc/build/v2/engine/modules/property-set.d \
./doc/build/v2/engine/modules/regex.d \
./doc/build/v2/engine/modules/sequence.d \
./doc/build/v2/engine/modules/set.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/engine/modules/%.o: ../doc/build/v2/engine/modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


