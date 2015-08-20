################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../examples/ex_agent.cpp \
../examples/ex_ea.cpp \
../examples/ex_ea_mpi.cpp \
../examples/ex_eps_moea.cpp \
../examples/ex_nsga2.cpp 

OBJS += \
./examples/ex_agent.o \
./examples/ex_ea.o \
./examples/ex_ea_mpi.o \
./examples/ex_eps_moea.o \
./examples/ex_nsga2.o 

CPP_DEPS += \
./examples/ex_agent.d \
./examples/ex_ea.d \
./examples/ex_ea_mpi.d \
./examples/ex_eps_moea.d \
./examples/ex_nsga2.d 


# Each subdirectory must supply rules for building sources it contributes
examples/%.o: ../examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


