################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Zmod/baremetal/reg/reg.c 

OBJS += \
./src/Zmod/baremetal/reg/reg.o 

C_DEPS += \
./src/Zmod/baremetal/reg/reg.d 


# Each subdirectory must supply rules for building sources it contributes
src/Zmod/baremetal/reg/%.o: ../src/Zmod/baremetal/reg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -ID:/Vitis/eclypse_0/export/eclypse_0/sw/eclypse_0/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


