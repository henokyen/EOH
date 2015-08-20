################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/generators-test/a.cpp \
../doc/build/v2/test/generators-test/e.cpp \
../doc/build/v2/test/generators-test/z.cpp 

CXX_SRCS += \
../doc/build/v2/test/generators-test/b.cxx 

OBJS += \
./doc/build/v2/test/generators-test/a.o \
./doc/build/v2/test/generators-test/b.o \
./doc/build/v2/test/generators-test/e.o \
./doc/build/v2/test/generators-test/z.o 

CPP_DEPS += \
./doc/build/v2/test/generators-test/a.d \
./doc/build/v2/test/generators-test/e.d \
./doc/build/v2/test/generators-test/z.d 

CXX_DEPS += \
./doc/build/v2/test/generators-test/b.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/generators-test/%.o: ../doc/build/v2/test/generators-test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

doc/build/v2/test/generators-test/%.o: ../doc/build/v2/test/generators-test/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


