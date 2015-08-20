################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/eval/mpi.cpp \
../tests/eval/parallel.cpp 

OBJS += \
./tests/eval/mpi.o \
./tests/eval/parallel.o 

CPP_DEPS += \
./tests/eval/mpi.d \
./tests/eval/parallel.d 


# Each subdirectory must supply rules for building sources it contributes
tests/eval/%.o: ../tests/eval/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


