################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc40xx.c \
../example/src/kinetic_mirror.c \
../example/src/sysinit.c 

C_DEPS += \
./example/src/cr_startup_lpc40xx.d \
./example/src/kinetic_mirror.d \
./example/src/sysinit.d 

OBJS += \
./example/src/cr_startup_lpc40xx.o \
./example/src/kinetic_mirror.o \
./example/src/sysinit.o 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c example/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M4 -D__REDLIB__ -DSDK_DEBUGCONSOLE=0 -I"/Users/evanblasband/Documents/MCUXpressoIDE_11.6.0_8187/workspace/lpc_chip_40xx/inc" -I"/Users/evanblasband/Documents/MCUXpressoIDE_11.6.0_8187/workspace/lpc_board_ea_devkit_4088/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-example-2f-src

clean-example-2f-src:
	-$(RM) ./example/src/cr_startup_lpc40xx.d ./example/src/cr_startup_lpc40xx.o ./example/src/kinetic_mirror.d ./example/src/kinetic_mirror.o ./example/src/sysinit.d ./example/src/sysinit.o

.PHONY: clean-example-2f-src

