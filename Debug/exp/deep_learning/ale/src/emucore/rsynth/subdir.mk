################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../exp/deep_learning/ale/src/emucore/rsynth/darray.c \
../exp/deep_learning/ale/src/emucore/rsynth/elements.c \
../exp/deep_learning/ale/src/emucore/rsynth/holmes.c \
../exp/deep_learning/ale/src/emucore/rsynth/opsynth.c \
../exp/deep_learning/ale/src/emucore/rsynth/phones.c \
../exp/deep_learning/ale/src/emucore/rsynth/phtoelm.c \
../exp/deep_learning/ale/src/emucore/rsynth/trie.c 

OBJS += \
./exp/deep_learning/ale/src/emucore/rsynth/darray.o \
./exp/deep_learning/ale/src/emucore/rsynth/elements.o \
./exp/deep_learning/ale/src/emucore/rsynth/holmes.o \
./exp/deep_learning/ale/src/emucore/rsynth/opsynth.o \
./exp/deep_learning/ale/src/emucore/rsynth/phones.o \
./exp/deep_learning/ale/src/emucore/rsynth/phtoelm.o \
./exp/deep_learning/ale/src/emucore/rsynth/trie.o 

C_DEPS += \
./exp/deep_learning/ale/src/emucore/rsynth/darray.d \
./exp/deep_learning/ale/src/emucore/rsynth/elements.d \
./exp/deep_learning/ale/src/emucore/rsynth/holmes.d \
./exp/deep_learning/ale/src/emucore/rsynth/opsynth.d \
./exp/deep_learning/ale/src/emucore/rsynth/phones.d \
./exp/deep_learning/ale/src/emucore/rsynth/phtoelm.d \
./exp/deep_learning/ale/src/emucore/rsynth/trie.d 


# Each subdirectory must supply rules for building sources it contributes
exp/deep_learning/ale/src/emucore/rsynth/%.o: ../exp/deep_learning/ale/src/emucore/rsynth/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


