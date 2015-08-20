################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../doc/build/v2/test/qt4/phonon.cpp \
../doc/build/v2/test/qt4/qt3support.cpp \
../doc/build/v2/test/qt4/qtassistant.cpp \
../doc/build/v2/test/qt4/qtcore.cpp \
../doc/build/v2/test/qt4/qtcorefail.cpp \
../doc/build/v2/test/qt4/qtgui.cpp \
../doc/build/v2/test/qt4/qthelp.cpp \
../doc/build/v2/test/qt4/qtmultimedia.cpp \
../doc/build/v2/test/qt4/qtnetwork.cpp \
../doc/build/v2/test/qt4/qtscript.cpp \
../doc/build/v2/test/qt4/qtscripttools.cpp \
../doc/build/v2/test/qt4/qtsql.cpp \
../doc/build/v2/test/qt4/qtsvg.cpp \
../doc/build/v2/test/qt4/qttest.cpp \
../doc/build/v2/test/qt4/qtwebkit.cpp \
../doc/build/v2/test/qt4/qtxml.cpp \
../doc/build/v2/test/qt4/qtxmlpatterns.cpp 

OBJS += \
./doc/build/v2/test/qt4/phonon.o \
./doc/build/v2/test/qt4/qt3support.o \
./doc/build/v2/test/qt4/qtassistant.o \
./doc/build/v2/test/qt4/qtcore.o \
./doc/build/v2/test/qt4/qtcorefail.o \
./doc/build/v2/test/qt4/qtgui.o \
./doc/build/v2/test/qt4/qthelp.o \
./doc/build/v2/test/qt4/qtmultimedia.o \
./doc/build/v2/test/qt4/qtnetwork.o \
./doc/build/v2/test/qt4/qtscript.o \
./doc/build/v2/test/qt4/qtscripttools.o \
./doc/build/v2/test/qt4/qtsql.o \
./doc/build/v2/test/qt4/qtsvg.o \
./doc/build/v2/test/qt4/qttest.o \
./doc/build/v2/test/qt4/qtwebkit.o \
./doc/build/v2/test/qt4/qtxml.o \
./doc/build/v2/test/qt4/qtxmlpatterns.o 

CPP_DEPS += \
./doc/build/v2/test/qt4/phonon.d \
./doc/build/v2/test/qt4/qt3support.d \
./doc/build/v2/test/qt4/qtassistant.d \
./doc/build/v2/test/qt4/qtcore.d \
./doc/build/v2/test/qt4/qtcorefail.d \
./doc/build/v2/test/qt4/qtgui.d \
./doc/build/v2/test/qt4/qthelp.d \
./doc/build/v2/test/qt4/qtmultimedia.d \
./doc/build/v2/test/qt4/qtnetwork.d \
./doc/build/v2/test/qt4/qtscript.d \
./doc/build/v2/test/qt4/qtscripttools.d \
./doc/build/v2/test/qt4/qtsql.d \
./doc/build/v2/test/qt4/qtsvg.d \
./doc/build/v2/test/qt4/qttest.d \
./doc/build/v2/test/qt4/qtwebkit.d \
./doc/build/v2/test/qt4/qtxml.d \
./doc/build/v2/test/qt4/qtxmlpatterns.d 


# Each subdirectory must supply rules for building sources it contributes
doc/build/v2/test/qt4/%.o: ../doc/build/v2/test/qt4/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


