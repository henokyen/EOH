################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../doc/build/v2/engine/boehm_gc/cord/cordbscs.c \
../doc/build/v2/engine/boehm_gc/cord/cordprnt.c \
../doc/build/v2/engine/boehm_gc/cord/cordtest.c \
../doc/build/v2/engine/boehm_gc/cord/cordxtra.c \
../doc/build/v2/engine/boehm_gc/cord/de.c \
../doc/build/v2/engine/boehm_gc/cord/de_win.c 

OBJS += \
./doc/build/v2/engine/boehm_gc/cord/cordbscs.o \
./doc/build/v2/engine/boehm_gc/cord/cordprnt.o \
./doc/build/v2/engine/boehm_gc/cord/cordtest.o \
./doc/build/v2/engine/boehm_gc/cord/cordxtra.o \
./doc/build/v2/engine/boehm_gc/cord/de.o \
./doc/build/v2/engine/boehm_gc/cord/de_win.o 

C_DEPS += \
./doc/build/v2/engine/boehm_gc/cord/cordbscs.d \
./doc/build/v2/engine/boehm_gc/cord/cordprnt.d \
./doc/build/v2/engine/boehm_gc/cord/cordtest.d \
./doc/build/v2/engine/boehm_gc/cord/cordxtra.d \
./doc/build/v2/engine/boehm_gc/cord/de.d \
./doc/build/v2/engine/boehm_gc/cord/de_win.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/engine/boehm_gc/cord/%.o: ../doc/build/v2/engine/boehm_gc/cord/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


