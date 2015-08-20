################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/ea/cmaes.cpp \
../tests/ea/dom_sort.cpp \
../tests/ea/eps_moea.cpp \
../tests/ea/nsga2.cpp \
../tests/ea/rank_simple.cpp 

OBJS += \
./tests/ea/cmaes.o \
./tests/ea/dom_sort.o \
./tests/ea/eps_moea.o \
./tests/ea/nsga2.o \
./tests/ea/rank_simple.o 

CPP_DEPS += \
./tests/ea/cmaes.d \
./tests/ea/dom_sort.d \
./tests/ea/eps_moea.d \
./tests/ea/nsga2.d \
./tests/ea/rank_simple.d 


# Each subdirectory must supply rules for building sources it contributes
tests/ea/%.o: ../tests/ea/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


