################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usb/device/source/usb_device_ch9.c \
../usb/device/source/usb_device_dci.c 

C_DEPS += \
./usb/device/source/usb_device_ch9.d \
./usb/device/source/usb_device_dci.d 

OBJS += \
./usb/device/source/usb_device_ch9.o \
./usb/device/source/usb_device_dci.o 


# Each subdirectory must supply rules for building sources it contributes
usb/device/source/%.o: ../usb/device/source/%.c usb/device/source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_LPC55S28JBD100=1 -DCPU_LPC55S28JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\board" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\component\timer" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\source" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\device\source\lpcip3511" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\include" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\utilities" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\drivers" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\device" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\LPC55S28\drivers" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\phy" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\component\uart" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\component\lists" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\device\include" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\device\source" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\component\osa" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\CMSIS" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\device\class" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\usb\device\class\hid" -I"E:\I_NXP\NXP55S28\VGN\C_FW\V01\FW\lpcxpresso55s28_dev_composite_hid_mouse_hid_keyboard_bm\component\gpio" -O1 -fno-common -g -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-usb-2f-device-2f-source

clean-usb-2f-device-2f-source:
	-$(RM) ./usb/device/source/usb_device_ch9.d ./usb/device/source/usb_device_ch9.o ./usb/device/source/usb_device_dci.d ./usb/device/source/usb_device_dci.o

.PHONY: clean-usb-2f-device-2f-source

