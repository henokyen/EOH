################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modules/nn2/bench_nn.cpp \
../modules/nn2/test_dnn.cpp \
../modules/nn2/test_dnn_ff.cpp \
../modules/nn2/test_esn.cpp \
../modules/nn2/test_hyper_nn.cpp \
../modules/nn2/test_mlp.cpp \
../modules/nn2/test_nn.cpp \
../modules/nn2/test_osc.cpp 

OBJS += \
./modules/nn2/bench_nn.o \
./modules/nn2/test_dnn.o \
./modules/nn2/test_dnn_ff.o \
./modules/nn2/test_esn.o \
./modules/nn2/test_hyper_nn.o \
./modules/nn2/test_mlp.o \
./modules/nn2/test_nn.o \
./modules/nn2/test_osc.o 

CPP_DEPS += \
./modules/nn2/bench_nn.d \
./modules/nn2/test_dnn.d \
./modules/nn2/test_dnn_ff.d \
./modules/nn2/test_esn.d \
./modules/nn2/test_hyper_nn.d \
./modules/nn2/test_mlp.d \
./modules/nn2/test_nn.d \
./modules/nn2/test_osc.d 


# Each subdirectory must supply rules for building sources it contributes
modules/nn2/%.o: ../modules/nn2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


