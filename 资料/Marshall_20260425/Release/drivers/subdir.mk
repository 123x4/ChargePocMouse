################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_ctimer.c \
../drivers/fsl_dma.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gint.c \
../drivers/fsl_gpio.c \
../drivers/fsl_iap.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_mrt.c \
../drivers/fsl_reset.c \
../drivers/fsl_spi.c \
../drivers/fsl_spi_dma.c \
../drivers/fsl_usart.c 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_ctimer.d \
./drivers/fsl_dma.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gint.d \
./drivers/fsl_gpio.d \
./drivers/fsl_iap.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_mrt.d \
./drivers/fsl_reset.d \
./drivers/fsl_spi.d \
./drivers/fsl_spi_dma.d \
./drivers/fsl_usart.d 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_ctimer.o \
./drivers/fsl_dma.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gint.o \
./drivers/fsl_gpio.o \
./drivers/fsl_iap.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_mrt.o \
./drivers/fsl_reset.o \
./drivers/fsl_spi.o \
./drivers/fsl_spi_dma.o \
./drivers/fsl_usart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_LPC55S28JBD100=1 -DCPU_LPC55S28JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\board" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\timer" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source\lpcip3511" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\utilities" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\device" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\LPC55S28\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\phy" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\uart" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\lists" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\osa" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\CMSIS" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class\hid" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\gpio" -O1 -fno-common -g -gdwarf-4 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_ctimer.d ./drivers/fsl_ctimer.o ./drivers/fsl_dma.d ./drivers/fsl_dma.o ./drivers/fsl_flexcomm.d ./drivers/fsl_flexcomm.o ./drivers/fsl_gint.d ./drivers/fsl_gint.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_iap.d ./drivers/fsl_iap.o ./drivers/fsl_inputmux.d ./drivers/fsl_inputmux.o ./drivers/fsl_mrt.d ./drivers/fsl_mrt.o ./drivers/fsl_reset.d ./drivers/fsl_reset.o ./drivers/fsl_spi.d ./drivers/fsl_spi.o ./drivers/fsl_spi_dma.d ./drivers/fsl_spi_dma.o ./drivers/fsl_usart.d ./drivers/fsl_usart.o

.PHONY: clean-drivers

