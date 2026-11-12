################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/uart/fsl_adapter_usart.c 

C_DEPS += \
./component/uart/fsl_adapter_usart.d 

OBJS += \
./component/uart/fsl_adapter_usart.o 


# Each subdirectory must supply rules for building sources it contributes
component/uart/%.o: ../component/uart/%.c component/uart/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_LPC55S28JBD100=1 -DCPU_LPC55S28JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\board" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\timer" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source\lpcip3511" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\utilities" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\device" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\LPC55S28\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\phy" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\uart" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\lists" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\osa" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\CMSIS" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class\hid" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\gpio" -O1 -fno-common -g -gdwarf-4 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-uart

clean-component-2f-uart:
	-$(RM) ./component/uart/fsl_adapter_usart.d ./component/uart/fsl_adapter_usart.o

.PHONY: clean-component-2f-uart

