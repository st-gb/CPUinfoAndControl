################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Controller/Linux.cpp \
../Controller/PumaStateCtrl.cpp \
../Controller/SAX2Handler.cpp \
../Controller/XMLAccess.cpp 

CPP_DEPS += \
./Controller/Linux.d \
./Controller/PumaStateCtrl.d \
./Controller/SAX2Handler.d \
./Controller/XMLAccess.d 

OBJS += \
./Controller/Linux.o \
./Controller/PumaStateCtrl.o \
./Controller/SAX2Handler.o \
./Controller/XMLAccess.o 


# Each subdirectory must supply rules for building sources it contributes
Controller/%.o: ../Controller/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


