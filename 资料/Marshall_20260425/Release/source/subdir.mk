################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/CTimer.c \
../source/composite.c \
../source/dma_config.c \
../source/driverapp_communication.c \
../source/flash_iap.c \
../source/flash_zb25v.c \
../source/fntab_mode.c \
../source/hall_spi.c \
../source/hid_keyboard.c \
../source/hid_mouse.c \
../source/led.c \
../source/macro.c \
../source/resume.c \
../source/semihost_hardfault.c \
../source/testapp_communication.c \
../source/usb_device_descriptor.c 

C_DEPS += \
./source/CTimer.d \
./source/composite.d \
./source/dma_config.d \
./source/driverapp_communication.d \
./source/flash_iap.d \
./source/flash_zb25v.d \
./source/fntab_mode.d \
./source/hall_spi.d \
./source/hid_keyboard.d \
./source/hid_mouse.d \
./source/led.d \
./source/macro.d \
./source/resume.d \
./source/semihost_hardfault.d \
./source/testapp_communication.d \
./source/usb_device_descriptor.d 

OBJS += \
./source/CTimer.o \
./source/composite.o \
./source/dma_config.o \
./source/driverapp_communication.o \
./source/flash_iap.o \
./source/flash_zb25v.o \
./source/fntab_mode.o \
./source/hall_spi.o \
./source/hid_keyboard.o \
./source/hid_mouse.o \
./source/led.o \
./source/macro.o \
./source/resume.o \
./source/semihost_hardfault.o \
./source/testapp_communication.o \
./source/usb_device_descriptor.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_LPC55S28JBD100=1 -DCPU_LPC55S28JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\board" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\timer" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source\lpcip3511" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\utilities" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\device" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\LPC55S28\drivers" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\phy" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\uart" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\lists" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\include" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\source" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\osa" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\CMSIS" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\usb\device\class\hid" -I"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\component\gpio" -O1 -fno-common -g -gdwarf-4 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/CTimer.d ./source/CTimer.o ./source/composite.d ./source/composite.o ./source/dma_config.d ./source/dma_config.o ./source/driverapp_communication.d ./source/driverapp_communication.o ./source/flash_iap.d ./source/flash_iap.o ./source/flash_zb25v.d ./source/flash_zb25v.o ./source/fntab_mode.d ./source/fntab_mode.o ./source/hall_spi.d ./source/hall_spi.o ./source/hid_keyboard.d ./source/hid_keyboard.o ./source/hid_mouse.d ./source/hid_mouse.o ./source/led.d ./source/led.o ./source/macro.d ./source/macro.o ./source/resume.d ./source/resume.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/testapp_communication.d ./source/testapp_communication.o ./source/usb_device_descriptor.d ./source/usb_device_descriptor.o

.PHONY: clean-source

