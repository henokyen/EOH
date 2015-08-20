################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modules/nn/bench_nn.cpp \
../modules/nn/test_arch_nn.cpp \
../modules/nn/test_dnn.cpp \
../modules/nn/test_dnn_ff.cpp \
../modules/nn/test_elman.cpp \
../modules/nn/test_hyper_nn.cpp \
../modules/nn/test_nn.cpp 

OBJS += \
./modules/nn/bench_nn.o \
./modules/nn/test_arch_nn.o \
./modules/nn/test_dnn.o \
./modules/nn/test_dnn_ff.o \
./modules/nn/test_elman.o \
./modules/nn/test_hyper_nn.o \
./modules/nn/test_nn.o 

CPP_DEPS += \
./modules/nn/bench_nn.d \
./modules/nn/test_arch_nn.d \
./modules/nn/test_dnn.d \
./modules/nn/test_dnn_ff.d \
./modules/nn/test_elman.d \
./modules/nn/test_hyper_nn.d \
./modules/nn/test_nn.d 


# Each subdirectory must supply rules for building sources it contributes
modules/nn/%.o: ../modules/nn/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


